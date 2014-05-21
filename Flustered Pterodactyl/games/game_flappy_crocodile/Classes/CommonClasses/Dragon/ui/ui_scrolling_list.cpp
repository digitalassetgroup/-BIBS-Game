#include "ui_scrolling_list.h"
#include "ig_interface.h"
#include "../utilities/ig_utilities.h"
#include "Core/CollectionHelper.h"

//#include "../objects/obj_base.h"	/* Included for TouchEvent */

using namespace cocos2d;
using namespace DracoGames;
using namespace DracoGames::Utils;

//#include "file_operation.h"

namespace ig_interface
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	IScrollingListDelegate::~IScrollingListDelegate(void)
	{
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ScrollingListItem::~ScrollingListItem(void)
	{
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	const ScrollingList::ItemId ScrollingList::InvalidItemId(SIZE_T_MAX);
	
	ScrollingList::ScrollingList(IScrollingListDelegate& delegateObject, const bool isHorizontal) :
		m_delegateObject(delegateObject),
		m_isFinalized(false),
		m_isHorizontal(isHorizontal),
		m_grabbedItemId(InvalidItemId),
		m_selectedItemId(InvalidItemId),
		m_scrollOffset(0.0f),
		m_itemSpacing(0.0f, 0.0f),
		m_itemHalfDimensions(50.0f, 50.0f),
		m_itemPositioning(gScreenSize.width/2, gScreenSize.height/2),
		m_slideStartOffset(0.0f),
		m_slideBounceAmount(0.0f),
		m_totalSlideTime(0.35f),
		m_slideTimer(-1.0f)
	{
		m_swipeInfo.startX = 0.0f;
		m_swipeInfo.startY = 0.0f;
		m_swipeInfo.currX = 0.0f;
		m_swipeInfo.currY = 0.0f;
		m_swipeInfo.touchTimer = -1.0f;
		m_swipeInfo.isTouching = false;
		m_swipeInfo.moved = false;
		
		autorelease();
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingList::~ScrollingList(void)
	{
		clearItems();
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingList* ScrollingList::makeScrollingList(IScrollingListDelegate& delegateObject, const bool isHorizontal)
	{
		assert_if(false == isHorizontal, "Not yet implemented; verticle scrolling lists.");		
		return new ScrollingList(delegateObject, isHorizontal);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::update(float deltaTime)
	{
		assertIfNotFinalized();
		
		if(true == m_swipeInfo.isTouching)
		{
			m_swipeInfo.touchTimer += deltaTime;
		}
		
		if(m_slideTimer >= 0.0f)
		{
			m_slideTimer -= deltaTime;
			
			const float slidePercentage(Utils::Clamp(1.0f - (m_slideTimer / m_totalSlideTime), 0.0f, 1.0f));
			const float interpBounce(Utils::Interp_CatMullRom(slidePercentage, m_slideBounceAmount, 0.0f, 1.0f, 1.0f));
			m_scrollOffset = m_slideStartOffset - (m_slideStartOffset * interpBounce);
			//printf("sliding: %.3f :: %.3f ... off: %.3f            sta: %.3f\n", slidePercentage, interpBounce, m_scrollOffset, m_slideStartOffset);
			repositionItems();
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	bool ScrollingList::handleTouch(const TouchEvent& evt)
	{
		assertIfNotFinalized();

		assert_if(false == Utils::isEqual(g_convertRatio->x, g_convertRatio->y), "The following two lines assumes Utils::isEqual(g_convertRatio->x, g_convertRatio->y) is true. ");
		const float MinSlideToUngrabItem(5.0f * g_convertRatio->x);
		const float MinSlideToChangeItem(10.0f * g_convertRatio->x);
		
		m_swipeInfo.currX = evt.atScreen.x;
		m_swipeInfo.currY = evt.atScreen.y;
		
		if(TouchPhase::Began == evt.phase)
		{
			if(false == isSwipeInBounds())
			{
				return false;
			}
			
			/**
			 *	This really should be capable of being an assert_if, but 
			 */
			warning_if(true == m_swipeInfo.isTouching, "Unexpected; isTouching is already true.");
			m_swipeInfo.startX = evt.atScreen.x;
			m_swipeInfo.startY = evt.atScreen.y;
			m_swipeInfo.moved = false;
			m_swipeInfo.isTouching = true;
			m_swipeInfo.touchTimer = 0.0f;
			
			grabClickedItem();
		}
		else if(TouchPhase::Moved == evt.phase)
		{			
			if(false == isSwipeInBounds() && true == m_swipeInfo.isTouching)
			{	/* The touch is out of bounds. */
				TouchEvent endEvt;
				endEvt.atScreen = evt.atScreen;
				endEvt.atWorld = evt.atWorld;
				endEvt.phase = TouchPhase::Ended;
				handleTouch(endEvt);
				
				return false;	/* Pretend we did nothing despite our efforts. */
			}
			else if(true == isSwipeInBounds() && false == m_swipeInfo.isTouching)
			{	/* The swip re-entered the swipeable area after leaving at one point in time. */
				TouchEvent endEvt;
				endEvt.atScreen = evt.atScreen;
				endEvt.atWorld = evt.atWorld;
				endEvt.phase = TouchPhase::Ended;
				handleTouch(endEvt);
			}
			else if(true == m_swipeInfo.isTouching)
			{	/* Actually handle this as if the touch moved within the swipeable area. */				
				const float swipeDistance(getSwipeDistance());
				if(abs(swipeDistance) >= MinSlideToUngrabItem)
				{
					m_swipeInfo.moved = true;
					ungrabItem();
				}

				m_scrollOffset = swipeDistance;				
				repositionItems();				
			}
			else
			{	/* The touch is out of bounds, do not handle it. */
				return false;
			}
		}
		else if(TouchPhase::Ended == evt.phase && true == m_swipeInfo.isTouching)
		{
			m_swipeInfo.isTouching = false;
			
			if(true == isSwipeInBounds() && false == m_swipeInfo.moved && InvalidItemId != m_grabbedItemId)
			{
				m_delegateObject.onClickItem();
			}
			else
			{
				const float swipeDistance(getSwipeDistance());
				const bool doBounce((m_swipeInfo.touchTimer < 0.2f && abs(swipeDistance) > MinSlideToChangeItem * 4.0f) ? true : false);
			
				if(swipeDistance >= MinSlideToChangeItem)
				{	/* Change the selection to the previous item, and slide to it. */
					slideToPrevItem(doBounce);
				}
				else if(swipeDistance <= -MinSlideToChangeItem)
				{	/* Change the selection to the next item, and slide to it. */
					slideToNextItem(doBounce);
				}
				else
				{	/* The selection did not change, but slide back to the item. */
					slideToCurrItem(doBounce);
				}
			}
			
			ungrabItem();
		}		
		
		return true;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::clearItems(void)
	{
		while(false == m_scrollingItems.empty())
		{
			removeChild(m_scrollingItems.back(), true);
			m_scrollingItems.pop_back();
		}
		
		while(false == m_deletableItems.empty())
		{
			SafeDelete(m_deletableItems.back());
			m_deletableItems.pop_back();
		}
		
		m_selectedItemId = InvalidItemId;
		m_grabbedItemId = InvalidItemId;
		
		const_cast<bool&>(m_isFinalized) = false;	/* This should not be changed at will; remains const elsewhere. */		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::addItem(ScrollingListItem* const listItem, const bool callDelete)
	{
		assertIfFinalized();
		assert_if(nullptr == listItem, "Invalid value for parameter; listItem.  Expected a non-null pointer.");
		
		m_scrollingItems.push_back(listItem);
		addChild(listItem);
		
		if(1 == m_scrollingItems.size())
		{	/* This is the first item added to the list, it becomes our selection. */
			setSelectedItem(0);
		}
		
		if(true == callDelete)
		{
			m_deletableItems.push_back(listItem);
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::finalize(void)
	{
		assert_if(true == m_scrollingItems.empty(), "Cannot have an empty scrolling list, should be at least 2 items.");
		assert_if(InvalidItemId == m_selectedItemId, "One of the items must be selected before finalizing the list.");
		assertIfFinalized();
		const_cast<bool&>(m_isFinalized) = true;	/* This should not be changed at will; remains const elsewhere. */
		repositionItems();
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::setSelectedItem(const ItemId& itemId)
	{
		assertIfFinalized();
		assert_if(itemId >= m_scrollingItems.size(), "Invalid value for parameter; itemId - Value out of range.");
		assert_if(itemId == InvalidItemId, "Cannot select an item with an invalid id.");

		const ItemId prevSelectedItemId(m_selectedItemId);
		m_selectedItemId = itemId;
		m_scrollOffset = 0.0f;
		
		if(prevSelectedItemId != m_selectedItemId)
		{
			m_delegateObject.onSelectedItemChanged();
		}
		
		/** repositionItems() if we decide to allow this to work after being finalized. */
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::setHorizontalSpacing(const float spacing)
	{
		assertIfFinalized();
		assert_if(spacing <= 0.0f, "Invalid value for parameter; spacing (%f).  Expected positive, non-zero value.\n", spacing);
		assert_if(false == m_isHorizontal, "This scrolling list is not horizontal.  Cannot have horizontal spacing.");
		m_itemSpacing.x = spacing;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::setVerticalSpacing(const float spacing)
	{
		assertIfFinalized();
		assert_if(spacing <= 0.0f, "Invalid value for parameter; spacing (%f).  Expected positive, non-zero value.\n", spacing);
		assert_if(true == m_isHorizontal, "This scrolling list is horizontal.  Cannot have verticle spacing.");
		m_itemSpacing.y = spacing;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::setSelectedItemPosition(const float centerX, const float centerY)
	{
		assertIfFinalized();
		assert_if(centerX < 0.0f || centerX > gScreenSize.width, "Invalid value for parameter; centerX (%f).  Expected 0.0f <= centerX <= %f\n", centerX, gScreenSize.width);
		assert_if(centerY < 0.0f || centerX > gScreenSize.height, "Invalid value for parameter; centerY (%f).  Expected 0.0f <= centerY <= %f\n", centerX, gScreenSize.height);		
		m_itemPositioning.x = centerX;
		m_itemPositioning.y = centerY;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::setItemDimensions(const float width, const float height)
	{
		assertIfFinalized();
		assert_if(width <= 0.0f, "Invalid value for parameter; width (%f).  Expected positive, non-zero value.\n", width);
		assert_if(height <= 0.0f, "Invalid value for parameter; height (%f).  Expected positive, non-zero value.\n", height);		
		m_itemHalfDimensions.x = width / 2.0f;
		m_itemHalfDimensions.y = height / 2.0f;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float ScrollingList::getHorizontalScrollOffset(const bool totalOffset) const
	{
		assertIfNotFinalized();
		
		if(false == m_isHorizontal)
		{	/* This is a vertical scrolling list, no horizontal offset is possible. */
			return 0.0f;
		}
		
		if(false == totalOffset)
		{		
			return m_scrollOffset;
		}
		
		return getHorizontalScrollOffset(false) - (m_selectedItemId * m_itemSpacing.x);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float ScrollingList::getVerticalScrollOffset(const bool totalOffset) const
	{
		assertIfNotFinalized();
		
		if(true == m_isHorizontal)
		{	/* This is a horizontal scrolling list, no vertical offset is possible. */
			return 0.0f;
		}

		if(false == totalOffset)
		{
			return m_scrollOffset;
		}
		
		return getVerticalScrollOffset(false) - (m_selectedItemId * m_itemSpacing.y);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	size_t ScrollingList::getItemCount(void) const
	{
		assertIfNotFinalized();		
		return m_scrollingItems.size();
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::repositionItems(void)
	{
		assertIfNotFinalized();
		
		cocos2d::CCPoint itemPosition;
		ItemId id(0);

		const float horizontalScrollOffset(getHorizontalScrollOffset(true));
		const float verticalScrollOffset(getVerticalScrollOffset(true));
		
		for_each(ScrollingListItem* item, m_scrollingItems)
		{
			itemPosition.x = (m_itemPositioning.x + (id * m_itemSpacing.x)) + horizontalScrollOffset;
			itemPosition.y = (m_itemPositioning.y + (id * m_itemSpacing.y)) + verticalScrollOffset;
			
			item->setPosition(itemPosition);
			
			++id;
		}
		
		m_delegateObject.onPositionChange();
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	float ScrollingList::getSwipeDistance(void) const
	{
		assertIfNotFinalized();
		
		if(true == m_isHorizontal)
		{
			return m_swipeInfo.currX - m_swipeInfo.startX;
		}
		
		return m_swipeInfo.currY - m_swipeInfo.startY;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::slideToPrevItem(const bool doBounce)
	{
		assertIfNotFinalized();
		
		if(m_selectedItemId > 0)
		{
			--m_selectedItemId;
			m_delegateObject.onSelectedItemChanged();
			m_scrollOffset -= m_itemSpacing.x;
			assert_if(false == m_isHorizontal, "Not taking into account m_itemSpacing.y if this is vertical\n");			
		}
		
		slideToCurrItem(doBounce);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::slideToCurrItem(const bool doBounce)
	{
		assertIfNotFinalized();
		
		m_slideStartOffset = m_scrollOffset;
		m_slideBounceAmount = (true == doBounce) ? -10.0f : 0.0f;
		m_slideTimer = m_totalSlideTime;
		m_scrollOffset = 0.0f;
		
		repositionItems();
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::slideToNextItem(const bool doBounce)
	{
		assertIfNotFinalized();
		
		if(m_selectedItemId < (m_scrollingItems.size() - 1))
		{
			++m_selectedItemId;
			m_delegateObject.onSelectedItemChanged();			
			m_scrollOffset += m_itemSpacing.x;
			assert_if(false == m_isHorizontal, "Not taking into account m_itemSpacing.y if this is vertical\n");
		}
		
		slideToCurrItem(doBounce);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::grabClickedItem(void)
	{
		assertIfNotFinalized();
		
		ScrollingListItem *item(m_scrollingItems[m_selectedItemId]);
		size_t grabbedId(m_selectedItemId);
		
		if (m_isHorizontal)
		{
			const float halfWidth(0.5f * m_itemSpacing.x);
			const float leftBound(item->getPositionX() - halfWidth);
			const float rightBound(item->getPositionX() + halfWidth);
			if (m_swipeInfo.startX < leftBound && grabbedId > 0)
			{
				--grabbedId;
			}
			else if (m_swipeInfo.startX > rightBound && (grabbedId + 1) < m_scrollingItems.size())
			{
				++grabbedId;
			}
		}
		grabItem(grabbedId);
	}	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ScrollingList::grabItem(const ItemId grabbedItemId)
	{
		assertIfNotFinalized();
		assert_if(InvalidItemId != m_grabbedItemId, "Must ungrab the current item before grabbing a new item.");
		assert_if(grabbedItemId >= m_scrollingItems.size(), "Invalid value for parameter; grabbedItemId. Out of range.");
		assert_if(grabbedItemId == InvalidItemId, "Cannot grab an item with an invalid id.");		
		
		m_grabbedItemId = grabbedItemId;
		m_delegateObject.onGrabItem();
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingList::ungrabItem(void)
	{
		assertIfNotFinalized();
		
		if(InvalidItemId == m_grabbedItemId)
		{	/* There is no item currently grabbed. */
			return;
		}
						
		m_delegateObject.onUngrabItem();
		m_grabbedItemId = InvalidItemId;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	bool ScrollingList::isSwipeInBounds(void) const
	{
		assertIfNotFinalized();
		assert_if(m_itemHalfDimensions.x <= 0.0f || m_itemHalfDimensions.y <= 0.0f, "Item dimensions are invalid.");
		
		if(true == m_isHorizontal)
		{
			const float y(m_swipeInfo.currY);
			if(y > m_itemPositioning.y - m_itemHalfDimensions.y && y < m_itemPositioning.y + m_itemHalfDimensions.y)
			{
				return true;
			}
			return false;
		}
		else
		{
			const float x(m_swipeInfo.currX);
			if(x > m_itemPositioning.x - m_itemHalfDimensions.x && x < m_itemPositioning.x + m_itemHalfDimensions.x)
			{
				return true;
			}
			return false;
		}
		return false;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	bool ScrollingList::isSwipeInItem(const ItemId& itemId) const
	{
		assertIfNotFinalized();
		assert_if(m_itemHalfDimensions.x <= 0.0f || m_itemHalfDimensions.y <= 0.0f, "Item dimensions are invalid.");	
		assert_if(true, "Not yet implemented: %d.\n", static_cast<int>(itemId));
		
		if(false == isSwipeInBounds())
		{
			return false;
		}

		return false;
	}

	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	void ScrollingList::assertIfFinalized(void) const
	{
		assert_if(true == m_isFinalized, "This can not happen after the ScrollingList has been finalized!\n");
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	void ScrollingList::assertIfNotFinalized(void) const
	{
		assert_if(true != m_isFinalized, "The scrolling list must be finalized() before this can happen.\n");
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
}; /* namespace ig_interface */
