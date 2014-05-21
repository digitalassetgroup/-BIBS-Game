#pragma once
#ifndef SUPER_KID_CANNON_SCROLLING_LIST_H
#define SUPER_KID_CANNON_SCROLLING_LIST_H "$Rev: 518 $"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cocos2d.h"
#include <string>
#include <vector>

#include "../utilities/ig_one_time_const.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ig_interface
{
	struct TouchEvent;

	/**
	 *	A way to figure out what is happening with the list selection, when selection changes, 
	 */
	class IScrollingListDelegate
	{
	public:
		virtual ~IScrollingListDelegate(void);
		
		/**
		 *	Called when the selected item actually changes based on user actions or calling setSelectedItem().
		 *
		 *	@note that the selection changes even while the selected item is offset from the position it will slide to.
		 */
		virtual void onSelectedItemChanged(void) { /* Base implementation does nothing. */ }
		
		/**
		 *	Will be called when the user grabs an item, and releases without sliding around too much.
		 *
		 *	@note getGrabbedItem() should be valid when this is called; to safely check the grabbed item
		 */
		virtual void onClickItem(void) { /* Base implementation does nothing. */ }
		
		/**
		 *	Called just after the grabbedItem changes from InvalidItemId to a valid item grabbed
		 *
		 *	@note getGrabbedItem() willbe valid when this is called; to safely check the grabbed item
		 */
		virtual void onGrabItem(void) { /* Base implementation does nothing. */ }
		
		/**
		 *	Called just before the grabbedItem changes from a valid item to InvalidItemId.
		 *
		 *	@note getGrabbedItem() will be valid when this is called; to safely check the grabbed item
		 */
		virtual void onUngrabItem(void) { /* Base implementation does nothing. */ }
		
		/**
		 *	This will be called when the list scrolls, items get added, item spacing or dimensions change or for any
		 *	other concievable reason why the position of the ScrollingListItems may need to be updated.
		 */
		virtual void onPositionChange(void) { /* Base implementation does nothing. */ }
	};
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	/**
	 *	An item or node within the list, managed/manipulated by the list.
	 */
	class ScrollingListItem : public cocos2d::CCNode
	{
	public:
		virtual ~ScrollingListItem(void);	
	};
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	/**
	 *	The list manages the items within to perform the magic scrolling.
	 */
	class ScrollingList : public cocos2d::CCNode
	{
	public:
		typedef size_t ItemId;
		static const ItemId InvalidItemId;
		
		virtual ~ScrollingList(void);

		static ScrollingList* makeScrollingList(IScrollingListDelegate& delegateObject, const bool isHorizontal = true);
		
		virtual void update(float deltaTime);
		
		/**
		 *	To be called when the touches change, move, begin, end, etc etc.  This allows the menu to handle any logic
		 *	for scrolling the items, and selecting items.
		 *
		 *	@return value is true if the touch was handled, menu scrolled/tapped etc.  Otherwise false is returned.
		 */
		bool handleTouch(const TouchEvent& evt);
		
		/**
		 *
		 */
		void clearItems(void);
		
		/**
		 *	@param callDelete is used to determine if the list should delete the item once the list dies.
		 *	@warning this parameter may be deprecated- listItem may always be deleted in the future.
		 */
		void addItem(ScrollingListItem* const listItem, const bool callDelete);
		
		/**
		 *	
		 */
		void finalize(void);
		
		/**
		 *	Selects an item and jumps places it in view immediately which may cause a snap - should only be called
		 *	during the initialization of the scrolling list / scene.
		 *
		 *	@warning this may cause the scrolling items to SNAP to a new position.
		 */
		void setSelectedItem(const ItemId& itemId);
		
		/**
		 *	Check what item is currently selected.
		 */
		inline const ItemId& getSelectedItem(void) const { return m_selectedItemId; }
		
		inline const ItemId& getGrabbedItem(void) const { return m_grabbedItemId; }
		
		inline ScrollingListItem& getItem(const ItemId& itemId) { return *m_scrollingItems[itemId]; }
		inline const ScrollingListItem& getItem(const ItemId& itemId) const { return *m_scrollingItems[itemId]; }
		
		/**
		 *	How much spacing exists between one item, and the item to the RIGHT of it.
		 *
		 *	@param spacing is expected to be a positive, non-zero value.
		 *	@warning this is only valid to call if the scrolling list was created as a horizontal list.
		 */
		void setHorizontalSpacing(const float spacing);

		/**
		 *	How much spacing exists between one item, and the item BELOW it.
		 *
		 *	@param spacing is expected to be a positive, non-zero value.
		 *	@warning this is only valid to call if the scrolling list was NOT created as a horizontal list.
		 */
		void setVerticalSpacing(const float spacing);
		
		/**
		 *	Set the position of the selected item, this value defaults to the center of the screen, but allows lowering
		 *	and raising for customizability.
		 */
		void setSelectedItemPosition(const float centerX, const float centerY);
		
		/**
		 *	Set the dimensions of the every item in the list, they all use the same dimensions regardless of the 
		 *	width/height that is reported by the bounding box.
		 */
		void setItemDimensions(const float width, const float height);
		
		/**
		 *	This function behaves in two slightly different ways pending the totalOffset parameter.  If totalOffset is
		 *	false, the returned value will be the offset is the selectedItem from the selectedItems desired position.
		 *
		 *	If totalOffset is true, the returned offset is how far the first item is from the position where the
		 *	selected item would want to be.  If the first item in the list was selected this would return the same
		 *	value as it would if totalOffset were false.  The total offset will be larger based on the spacing between
		 *	items and actual selected item.
		 *
		 *	@note will always return 0.0f if the scrolling list is a vertical scrolling list.
		 */
		float getHorizontalScrollOffset(const bool totalOffset) const;

		/**
		 *	This function behaves in two slightly different ways pending the totalOffset parameter.  If totalOffset is
		 *	false, the returned value will be the offset is the selectedItem from the selectedItems desired position.
		 *
		 *	If totalOffset is true, the returned offset is how far the first item is from the position where the
		 *	selected item would want to be.  If the first item in the list was selected this would return the same
		 *	value as it would if totalOffset were false.  The total offset will be larger based on the spacing between
		 *	items and actual selected item.
		 *
		 *	@note will always return 0.0f if the scrolling list is a horizontal scrolling list.
		 */
		float getVerticalScrollOffset(const bool totalOffset) const;		
		
		/**
		 *	Counts how many items are in the scrolling list, returning that value.
		 */
		size_t getItemCount(void) const;
		
	private:
		/* Prevent accidental use.  To Create a scrolling list use the static Factory methods. */
		ScrollingList(IScrollingListDelegate& delegateObject, const bool isHorizontal);
		ScrollingList(const ScrollingList& obj);
		ScrollingList& operator=(const ScrollingList& obj);

		/**
		 *	Sets the positions of each of the items based on the itemSpacing, itemPosition, scrollOffset and the
		 *	selectedItem.  If any of these values change, this function should be called.
		 */
		void repositionItems(void);
		
		/**
		 *	Computes the swipe distance from the swiping information pending the horizontal or vertical scrolling.
		 */
		float getSwipeDistance(void) const;
		
		void slideToPrevItem(const bool doBounce);
		void slideToCurrItem(const bool doBounce);
		void slideToNextItem(const bool doBounce);
		
		/**
		 * Grabs selected item or it's neighbor depending on click position
		 */
		void grabClickedItem();		
		void grabItem(const ItemId grabbedItemId);
		void ungrabItem(void);

		bool isSwipeInBounds(void) const;
		bool isSwipeInItem(const ItemId& itemId) const;

		void assertIfFinalized(void) const;		
		void assertIfNotFinalized(void) const;
		
		struct SwipingInfo
		{
			float startX;
			float startY;
			float currX;
			float currY;
			float touchTimer;			
			bool moved;
			bool isTouching;
		};
		
		std::vector<ScrollingListItem*> m_scrollingItems;
		std::vector<ScrollingListItem*> m_deletableItems;
		
		SwipingInfo m_swipeInfo;
		IScrollingListDelegate& m_delegateObject;
		
		cocos2d::CCPoint m_itemSpacing;
		cocos2d::CCPoint m_itemPositioning;
		cocos2d::CCPoint m_itemHalfDimensions;
		ItemId m_selectedItemId;
		ItemId m_grabbedItemId;
		float m_scrollOffset;
		const bool m_isHorizontal;
		const bool m_isFinalized;		/* This is mostly constant, can change ONLY in clearItems and finalize. */
		
		//
		float m_slideStartOffset;
		float m_slideBounceAmount;
		const float m_totalSlideTime;	/* This is a pseudo constant that can change with a const_cast. */
		float m_slideTimer;
	};
	
}; /* namespace ig_interface */

#endif /* SUPER_KID_CANNON_SCROLLING_LIST_H */
