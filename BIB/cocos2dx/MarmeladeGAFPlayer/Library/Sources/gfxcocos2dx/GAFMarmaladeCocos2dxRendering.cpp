#include "cocoa/CCAffineTransform.h"
#include "CCScheduler.h"
#include "platform/CCImage.h"
#include "textures/CCTexture2D.h"
#include "sprite_nodes/CCSpriteFrame.h"
#include "sprite_nodes/CCSpriteBatchNode.h"
#include "layers_scenes_transitions_nodes/CCLayer.h"
#include "sprite_nodes/CCSprite.h"

#if CC_SPRITEBATCHNODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__A__) ( (int)(__A__))
#endif

#include "CCDirector.h"
#include "misc_nodes/CCRenderTexture.h"
#include "platform/CCFileUtils.h"
#include "shaders/CCGLProgram.h"
#include "shaders/CCGLProgram.h"
#include "shaders/CCShaderCache.h"
#include "shaders/ccShaders.h"
#include "ccTypes.h"

#include "GAFMarmalade.h"
#include "GAFShaderProgram.h"
#include "GAFData.h"
#include "GAFAffineTransform.h"
#include "GAFSprite.h"

#include <algorithm>
#include <map>

using cocos2d::CCLog;

#define  kGAFStencilMaskAlphaFilterProgramCacheKey "kGAFScrollLayerAlphaFilterProgramCacheKey"
static const char * kAlphaFragmentShaderFilename = "pcShader_PositionTextureAlpha_frag.fs";

namespace GAF
{
	void * GAFMarmaladeGFX::createImage(const char * path)
	{
		cocos2d::CCImage * image = new cocos2d::CCImage();
		if (!image->initWithImageFile(path))
		{
			delete image;
			image = 0;
		}
		return image;
	}
	
	void GAFMarmaladeGFX::releaseImage(void * image)
	{
		if (!image)
		{
			return;
		}
		cocos2d::CCImage * img = (cocos2d::CCImage *) image;
		img->release();
	}
	
	void * GAFMarmaladeGFX::createTexture(void * image)
	{
		if (!image)
		{
			return NULL;
		}
		cocos2d::CCImage * img = (cocos2d::CCImage *) image;
		cocos2d::CCTexture2D * texture = new cocos2d::CCTexture2D();
		if (!texture->initWithImage(img))
		{
			delete texture;
			texture = 0;
		}
		return texture;
	}
	
	void GAFMarmaladeGFX::releaseTexture(void * texture)
	{
		if (!texture)
		{
			return;
		}
		cocos2d::CCTexture2D * tex = (cocos2d::CCTexture2D *) texture;
		tex->release();
	}
	
	void * GAFMarmaladeGFX::createSpriteContainer()
	{
		return new cocos2d::CCLayer();
	}
	
	void GAFMarmaladeGFX::releaseSpriteContainer(void * spriteContaner)
	{
		if (!spriteContaner)
		{
			return;
		}
		cocos2d::CCLayer * layer = (cocos2d::CCLayer *) spriteContaner;
		layer->release();
	}
	
	cocos2d::CCGLProgram * createWithFragmentFilename(const char * vertexSource, const char * fragmentFilename)
	{
		if (!vertexSource || !fragmentFilename)
		{
			return NULL;
		}
		std::string fp = cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(fragmentFilename);
		GAFData data;
		data.setDeleteData(true);
		unsigned long size;
		data.setBytes(cocos2d::CCFileUtils::sharedFileUtils()->getFileData(fp.c_str(), "r", &size));
		data.setSize(size);
		if (!data.bytes())
		{
			CCLOGERROR("Cannot load fragment shader with name %s", fragmentFilename);
			return NULL;
		}
		cocos2d::CCGLProgram * res = new cocos2d::CCGLProgram();
		if (!res)
		{
			return NULL;
		}
		if (data.size())
		{
			data.bytes()[data.size() - 1] = 0;
		}
		
		if (!res->initWithVertexShaderByteArray(vertexSource, (const char *)data.bytes()))
		{
			delete res;
			return NULL;
		}
		return res;
	}
	
	void * createShader(GAF::GAFGLProgramType type)
	{
		if (GAF::CCGLPROGRAM_ALPHA_SPRITE_SHADER == type)
		{
			cocos2d::CCGLProgram * program = createWithFragmentFilename(cocos2d::ccPositionTextureColor_vert, kAlphaFragmentShaderFilename);
			if (program)
			{
				program->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
				program->addAttribute(kCCAttributeNameColor,    cocos2d::kCCVertexAttrib_Color);
				program->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);
				program->link();
				program->updateUniforms();
				CHECK_GL_ERROR_DEBUG();
			}
			return program;
		}
		return NULL;
	}
	
	void   releaseShader(void * shader)
	{
		cocos2d::CCGLProgram * s = (cocos2d::CCGLProgram *)shader;
	}

#define kGAFSpriteWithAlphaShaderProgramCacheKey "kGAFSpriteWithAlphaShaderProgramCache"
	
class GAFTextureEffectsConverter;
static GAFTextureEffectsConverter * _sharedConverter = NULL;
	
	class GAFTextureEffectsConverter
	{
	private:
		cocos2d::CCDictionary * _vertexShaderUniforms;
	public:
		GAFTextureEffectsConverter()
		{
			_vertexShaderUniforms = new cocos2d::CCDictionary();
		}

		~GAFTextureEffectsConverter()
		{
			CC_SAFE_RELEASE(_vertexShaderUniforms);
		}
		static GAFTextureEffectsConverter * sharedConverter()
		{
			if (!_sharedConverter)
			{
				_sharedConverter = new GAFTextureEffectsConverter();
			}
			return _sharedConverter;
		}

		cocos2d::CCRenderTexture * gaussianBlurredTextureFromTexture(cocos2d::CCTexture2D * aTexture, const cocos2d::CCRect& rect, float aBlurRadiusX, float aBlurRadiusY)
		{
			const int kGaussianKernelSize = 9;
			
			cocos2d::CCSize rTextureSize = cocos2d::CCSizeMake(rect.size.width + 2 * (kGaussianKernelSize / 2) * aBlurRadiusX,
											 rect.size.height + 2 * (kGaussianKernelSize / 2) * aBlurRadiusY);
			
			cocos2d::CCRenderTexture *rTexture1 = cocos2d::CCRenderTexture::create(rTextureSize.width, rTextureSize.height);
			cocos2d::CCRenderTexture *rTexture2 = cocos2d::CCRenderTexture::create(rTextureSize.width, rTextureSize.height);
			cocos2d::CCGLProgram * shader = programForBlurShaderWithName("GaussianBlur", "Shaders/GaussianBlurVertexShader.vs", "Shaders/GaussianBlurFragmentShader.fs");
			if (!shader)
			{
				return NULL;
			}
			GLint texelWidthOffset = (GLint)glGetUniformLocation(shader->getProgram(), "texelWidthOffset");
			GLint texelHeightOffset = (GLint)glGetUniformLocation(shader->getProgram(), "texelHeightOffset");
			CHECK_GL_ERROR_DEBUG();
			{
				cocos2d::CCSprite *sprite = cocos2d::CCSprite::createWithTexture(aTexture, rect);
				sprite->setPosition(cocos2d::CCPointMake(rTextureSize.width / 2, rTextureSize.height / 2));				
				cocos2d::ccBlendFunc blend = { GL_ONE, GL_ZERO };
				sprite->setBlendFunc(blend);
				rTexture2->beginWithClear(0, 0, 0, 0);
				sprite->visit();
				rTexture2->end();
			}
			CHECK_GL_ERROR_DEBUG();
			{
				// Render rTexture2 to rTexture1 (horizontal)
				GLfloat texelWidthValue = aBlurRadiusX / (GLfloat)rTextureSize.width;
				GLfloat texelHeightValue = 0;
				
				rTexture2->getSprite()->setPosition(cocos2d::CCPointMake(rTextureSize.width / 2, rTextureSize.height / 2));
				rTexture2->getSprite()->setShaderProgram(shader);
				shader->use();
				glUniform1f(texelWidthOffset, texelWidthValue);
				glUniform1f(texelHeightOffset, texelHeightValue);
				cocos2d::ccBlendFunc blend = { GL_ONE, GL_ZERO };
				rTexture2->getSprite()->setBlendFunc(blend);
				rTexture1->beginWithClear(0, 0, 0, 0);
				rTexture2->getSprite()->visit();
				rTexture1->end();
			}
			
			CHECK_GL_ERROR_DEBUG();
			{
				// Render rTexture1 to rTexture2 (vertical)
				GLfloat texelWidthValue = 0;
				GLfloat texelHeightValue = aBlurRadiusY / (GLfloat)rTextureSize.height;
				
				rTexture1->getSprite()->setPosition(cocos2d::CCPointMake(rTextureSize.width / 2, rTextureSize.height / 2));
				rTexture1->getSprite()->setShaderProgram(shader);
				shader->use();
				glUniform1f(texelWidthOffset, texelWidthValue);
				glUniform1f(texelHeightOffset, texelHeightValue);
				cocos2d::ccBlendFunc blend = { GL_ONE, GL_ZERO };
				rTexture1->getSprite()->setBlendFunc(blend);
				rTexture2->beginWithClear(0, 0, 0, 0);
				rTexture1->getSprite()->visit();
				rTexture2->end();
			}
			CHECK_GL_ERROR_DEBUG();
			return rTexture2;
		}
		
		cocos2d::CCGLProgram * programForBlurShaderWithName(const char * aShaderName, const char * aVertexShaderFile, const char * aFragmentShaderFile)
		{
			cocos2d::CCGLProgram *program = cocos2d::CCShaderCache::sharedShaderCache()->programForKey(aShaderName);
			if (!program)
			{
				program = new cocos2d::CCGLProgram();
				program->initWithVertexShaderFilename(aVertexShaderFile, aFragmentShaderFile);
				if (program)
				{
					program->addAttribute("position", cocos2d::kCCVertexAttrib_Position);
					program->addAttribute("inputTextureCoordinate", cocos2d::kCCVertexAttrib_TexCoords);
					program->link();
					program->updateUniforms();
					CHECK_GL_ERROR_DEBUG();
					cocos2d::CCShaderCache::sharedShaderCache()->addProgram(program, aShaderName);
				}
				else
				{
					GAFLOGERROR("Cannot load program for %s.", aShaderName);
					return NULL;
				}
			}
			return program;
		}
	};
	
	
	struct GAFBlendFuncSeparate
	{
		// source blend function
		unsigned int src;
		// destination blend function
		unsigned int dst;
		// source alpha channel blend function
		unsigned int srcAlpha;
		// destination alpha channel blend function
		unsigned int dstAlpha;
	};
	
	class GAFCocos2dxSprite : public cocos2d::CCSprite
	{
	private:
		bool _isLocator;
		GAFBlendFuncSeparate _blendFuncSeparate;
		bool _useSeparateBlendFunc;
		int _blendEquation;
		float _atlasScale;
	protected:
		GAFSpriteCallback * _cb;
		cocos2d::CCAffineTransform _transform;
	public:
		GAFCocos2dxSprite(GAFSpriteCallback * cb)
		:
		_isLocator(false),
		_useSeparateBlendFunc(false),
		_blendEquation(-1),
		_cb(cb),
		_transform(cocos2d::CCAffineTransformMakeIdentity()),
		_atlasScale(1.0f)
		{
		}

		virtual void setUniformsForFragmentShader() = 0;
		
		inline void setLocator(bool locator)
		{
			_isLocator = locator;
		}
		
		void setTransform(const GAF::CCAffineTransform& transform)
		{
			cocos2d::CCAffineTransform t;
			t.a = transform.a;
			t.b = transform.b;
			t.c = transform.c;
			t.d = transform.d;
			t.tx = transform.tx;
			t.ty = transform.ty;
			if (!cocos2d::CCAffineTransformEqualToTransform(_transform, t))
			{
				_transform = t;
				m_bTransformDirty = true;
				m_bInverseDirty = true;
			}
		}
		
		void setAtlasScale(float scale)
		{
			if (_atlasScale != scale)
			{
				_atlasScale = scale;
				m_bTransformDirty = true;
				m_bInverseDirty = true;
			}
		}
		
		cocos2d::CCAffineTransform nodeToParentTransform(void)
		{
			if (m_bTransformDirty)
			{
				cocos2d::CCAffineTransform t = _transform;
				if (_atlasScale != 1.0f)
				{
					t = CCAffineTransformScale(t, _atlasScale, _atlasScale);
				}
				m_sTransform = CCAffineTransformTranslate(t, -m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
				m_bTransformDirty = false;
			}
			return m_sTransform;
		}
		
		void draw(void)
		{
			if (_isLocator)
			{
				return;
			}
			
			CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "GAFSprite - draw");
			
			CCAssert(!m_pobBatchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");
			
			CC_NODE_DRAW_SETUP();
			
			if (_useSeparateBlendFunc)
			{
				glBlendFuncSeparate(_blendFuncSeparate.src,      _blendFuncSeparate.dst,
									_blendFuncSeparate.srcAlpha, _blendFuncSeparate.dstAlpha);
			}
			else
			{
				cocos2d::ccGLBlendFunc(m_sBlendFunc.src, m_sBlendFunc.dst);
			}
			
			if (_blendEquation != -1)
			{
				glBlendEquation(_blendEquation);
			}
			
			if (m_pobTexture != NULL)
			{
				cocos2d::ccGLBindTexture2D( m_pobTexture->getName() );
			}
			else
			{
				cocos2d::ccGLBindTexture2D(0);
			}
			cocos2d::ccGLEnableVertexAttribs( cocos2d::kCCVertexAttribFlag_PosColorTex );
			setUniformsForFragmentShader();
			CHECK_GL_ERROR_DEBUG();
			
#define kQuadSize sizeof(m_sQuad.bl)
			long offset = (long)&m_sQuad;
			
			// vertex
			int diff = offsetof( cocos2d::ccV3F_C4B_T2F, vertices);
			glVertexAttribPointer(cocos2d::kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));
			
			// texCoods
			diff = offsetof( cocos2d::ccV3F_C4B_T2F, texCoords);
			glVertexAttribPointer(cocos2d::kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));
			
			// color
			diff = offsetof( cocos2d::ccV3F_C4B_T2F, colors);
			glVertexAttribPointer(cocos2d::kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));
			
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
			CHECK_GL_ERROR_DEBUG();
			
			CC_INCREMENT_GL_DRAWS(1);
			
			CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "GAFSprite - draw");
		}
		
		void setTexture(cocos2d::CCTexture2D *texture)
		{
			// If batchnode, then texture id should be the same
			CCAssert(! m_pobBatchNode || texture->getName() == m_pobBatchNode->getTexture()->getName(), "CCSprite: Batched sprites should use the same texture as the batchnode");
			// accept texture==nil as argument
			CCAssert( !texture || dynamic_cast<cocos2d::CCTexture2D*>(texture), "setTexture expects a CCTexture2D. Invalid argument");
			
			if (!m_pobBatchNode && m_pobTexture != texture)
			{
				CC_SAFE_RETAIN(texture);
				CC_SAFE_RELEASE(m_pobTexture);
				m_pobTexture = texture;
				updateBlendFunc();
			}
		}
		
		bool initWithTexture(cocos2d::CCTexture2D *pTexture, const cocos2d::CCRect& rect, bool rotated)
		{
			if (cocos2d::CCSprite::initWithTexture(pTexture, rect, rotated))
			{
				setShaderProgram(cocos2d::CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));
				return true;
			}
			else
			{
				return false;
			}
		}

	};
	
	class GAFMarmaladeCocos2dxSprite : public GAFCocos2dxSprite
	{
	public:
		GAFMarmaladeCocos2dxSprite(GAFSpriteCallback * cb)
		:
		GAFCocos2dxSprite(cb),
		_initialTexture(NULL),
		_colorTrasformLocation(-1)
		{
			
		}
		
		~GAFMarmaladeCocos2dxSprite()
		{
			CC_SAFE_RELEASE(_initialTexture);
		}
		
		void setUniformsForFragmentShader()
		{
			float colorTransform[8];
			_cb->getParameter(0, colorTransform);
			glUniform4fv(_colorTrasformLocation, 2, colorTransform);
		}
		
		void invalidateTransformCache()
		{
			m_bTransformDirty = true;
		}
		
		bool initWithTexture(cocos2d::CCTexture2D *pTexture, const cocos2d::CCRect& rect, bool rotated)
		{
			if (GAFCocos2dxSprite::initWithTexture(pTexture, rect, rotated))
			{
				_initialTexture = pTexture;
				_initialTexture->retain();
				_initialTextureRect = rect;
				_blurRadius = cocos2d::CCSizeZero;
				GAFMarmaladeGFX::setAlphaBlendingForSprite(this);
				setShaderProgram(programForShader());
				return true;
			}
			else
			{
				return false;
			}
		}
		
		cocos2d::CCGLProgram * programForShader()
		{
			cocos2d::CCGLProgram * program = cocos2d::CCShaderCache::sharedShaderCache()->programForKey(kGAFSpriteWithAlphaShaderProgramCacheKey);
			if (!program)
			{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT && !defined(_DEBUG))
#include "ShadersPrecompiled/GAFPrecompiledShaders.h"
				program = new CCGLProgram();
				program->autorelease();
				program->initWithPrecompiledProgramByteArray((const GLchar*)kGAFSpriteWithAlphaShaderProgramCache,
															 sizeof(kGAFSpriteWithAlphaShaderProgramCache));
				program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
				program->addAttribute(kCCAttributeNameColor,    kCCVertexAttrib_Color);
				program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
				program->updateUniforms();
				CHECK_GL_ERROR_DEBUG();
				CCShaderCache::sharedShaderCache()->addProgram(program, kGAFSpriteWithAlphaShaderProgramCacheKey);
#else
				
				program = createWithFragmentFilename(cocos2d::ccPositionTextureColor_vert, kAlphaFragmentShaderFilename);
				if (program)
				{
					program->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
					program->addAttribute(kCCAttributeNameColor,    cocos2d::kCCVertexAttrib_Color);
					program->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);
					program->link();
					program->updateUniforms();
					CHECK_GL_ERROR_DEBUG();
					cocos2d::CCShaderCache::sharedShaderCache()->addProgram(program, kGAFSpriteWithAlphaShaderProgramCacheKey);
				}
				else
				{
					CCLOGERROR("Cannot load program for GAFSpriteWithAlpha.");
					CC_SAFE_DELETE(program);
					return NULL;
				}
#endif				
			}
			program->use();
			_colorTrasformLocation = (GLuint)glGetUniformLocation(program->getProgram(), "colorTransform");
			if (_colorTrasformLocation <= 0)
			{
				CCLOGERROR("Cannot load program for GAFSpriteWithAlpha.");
			}
			return program;
		}

		
		void setBlurRadius(float width, float height)
		{
			if (0 == width && 0 == height)
			{
				setTexture(_initialTexture);
				setTextureRect(_initialTextureRect, false, _initialTextureRect.size);
				setFlipY(false);
			}
			else
			{
				GAFTextureEffectsConverter * converter = GAFTextureEffectsConverter::sharedConverter();
				cocos2d::CCRenderTexture * resultTex = converter->gaussianBlurredTextureFromTexture(_initialTexture, _initialTextureRect, width, height);
				if (resultTex)
				{
					setTexture(resultTex->getSprite()->getTexture());
					setFlipY(true);
					cocos2d::CCRect texureRect = cocos2d::CCRectMake(0, 0, resultTex->getSprite()->getContentSize().width, resultTex->getSprite()->getContentSize().height);
					setTextureRect(texureRect, false, texureRect.size);
				}
			}
		}
		
	private:
		cocos2d::CCSize        _blurRadius;
		cocos2d::CCTexture2D * _initialTexture;
		cocos2d::CCRect        _initialTextureRect;
		unsigned int           _colorTrasformLocation;
	};
	
	class GAFMarmaladeCocos2dxStencilSprite;
	static const char * kPCStencilMaskAlphaFilterFragmentShaderFilename = "Shaders/pcShader_PositionTexture_alphaFilter.fs";
	static std::map<cocos2d::CCNode *, GAFMarmaladeCocos2dxStencilSprite *> _object2maskedContainer;

	class GAFMarmaladeCocos2dxStencilSprite : public GAFCocos2dxSprite
	{
		
	private:
		cocos2d::CCArray * _maskedObjects;
		bool     _isReorderMaskedObjectsDirty;

	public:
		static bool compare_stencil_sprites(const void* p1, const void* p2)
		{
			GAFCocos2dxSprite* sp1 = (GAFCocos2dxSprite*)p1;
			GAFCocos2dxSprite* sp2 = (GAFCocos2dxSprite*)p2;
			
			return sp1->getZOrder() < sp2->getZOrder();
		}
		
		GAFMarmaladeCocos2dxStencilSprite(GAFSpriteCallback * cb)
		:
		GAFCocos2dxSprite(cb),
		_maskedObjects(NULL),
		_isReorderMaskedObjectsDirty(false)
		{
		}
		
		~GAFMarmaladeCocos2dxStencilSprite()
		{
			if (_maskedObjects)
			{
				for (unsigned int i = 0; i < _maskedObjects->count(); ++i)
				{
					GAFMarmaladeCocos2dxSprite * node = (GAFMarmaladeCocos2dxSprite*)_maskedObjects->objectAtIndex(i);
					std::map<cocos2d::CCNode *, GAFMarmaladeCocos2dxStencilSprite *>::iterator it = _object2maskedContainer.find(node);
					if (it != _object2maskedContainer.end())
					{
						_object2maskedContainer.erase(it);
					}
				}
			}
			
			CC_SAFE_RELEASE(_maskedObjects);
		}
	
		
		static void updateMaskContainerOf(GAFMarmaladeCocos2dxSprite * node)
		{
			std::map<cocos2d::CCNode *, GAFMarmaladeCocos2dxStencilSprite *>::iterator it = _object2maskedContainer.find(node);
			if (it != _object2maskedContainer.end())
			{
				it->second->invalidateMaskedObjectsOrder();
			}
		}
		
		
		bool initWithTexture(cocos2d::CCTexture2D *pTexture, const cocos2d::CCRect& rect, bool rotated)
		{
			if (!GAFCocos2dxSprite::initWithTexture(pTexture, cocos2d::CCRectMake(0, 0, rect.size.width, rect.size.height), rotated))
			{
				return false;
			}
			CC_SAFE_RELEASE(_maskedObjects);
			_maskedObjects = new cocos2d::CCArray();
			setShaderProgram(programShaderForMask());
			_isReorderMaskedObjectsDirty = false;
			return true;
		}
		
		void setUniformsForFragmentShader()
		{
		}
		
		
		void visit()
		{
			GAFCocos2dxSprite::visit();
			
			sortAllMaskedObjects();
			// Draw subobjects, assuming mask and object are on the same layer

			for (unsigned int i = 0; i < _maskedObjects->count(); ++i)
			{
				GAFMarmaladeCocos2dxSprite * object = (GAFMarmaladeCocos2dxSprite *) _maskedObjects->objectAtIndex(i);
				object->visit();
			}
			glDisable(GL_STENCIL_TEST);
		}
		
		void sortAllMaskedObjects()
		{
			if (_isReorderMaskedObjectsDirty)
			{
				std::sort(_maskedObjects->data->arr,
						  _maskedObjects->data->arr + _maskedObjects->data->num,
						  compare_stencil_sprites);
				_isReorderMaskedObjectsDirty = false;
			}
		}
		
		void draw()
		{
			// Prepare stencil
			glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);
			
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
			glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 1);
			
			// Draw mask
			GAFCocos2dxSprite::draw();
			
			// Use stencil
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glStencilFunc(GL_EQUAL, 1, 1);
		}
		
		void addMaskedObject(GAFCocos2dxSprite * anObject)
		{
			std::map<CCNode *, GAFMarmaladeCocos2dxStencilSprite *>::iterator it = _object2maskedContainer.find(anObject);
			
			GAFMarmaladeCocos2dxStencilSprite * maskContainer = it != _object2maskedContainer.end() ? it->second : NULL;
			
			if (maskContainer)
			{
				maskContainer->removeMaskedObject(anObject);
			}
			
			it = _object2maskedContainer.find(anObject);
			
			maskContainer = it != _object2maskedContainer.end() ? it->second : NULL;
			
			if (maskContainer != this)
			{
				_object2maskedContainer[anObject] = this;
				_maskedObjects->addObject(anObject);
				_isReorderMaskedObjectsDirty = true;
			}
		}
		
		void removeMaskedObject(GAFCocos2dxSprite * anObject)
		{
			if (_maskedObjects->containsObject(anObject))
			{
				std::map<CCNode *, GAFMarmaladeCocos2dxStencilSprite *>::iterator it = _object2maskedContainer.find(anObject);
				CCAssert(it != _object2maskedContainer.end(), "iterator must be valid");
				_object2maskedContainer.erase(it);
				_maskedObjects->removeObject(anObject);
				_isReorderMaskedObjectsDirty = true;
			}
		}
		
		void invalidateMaskedObjectsOrder()
		{
			_isReorderMaskedObjectsDirty = true;
		}
		
		cocos2d::CCGLProgram * programShaderForMask()
		{
			cocos2d::CCGLProgram *program = cocos2d::CCShaderCache::sharedShaderCache()->programForKey(kGAFStencilMaskAlphaFilterProgramCacheKey);
			
			if (!program)
			{
				program = createWithFragmentFilename(cocos2d::ccPositionTextureColor_vert, kPCStencilMaskAlphaFilterFragmentShaderFilename);
				if (program)
				{
					program->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
					program->addAttribute(kCCAttributeNameColor,    cocos2d::kCCVertexAttrib_Color);
					program->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);
					program->link();
					program->updateUniforms();
					CHECK_GL_ERROR_DEBUG();
					cocos2d::CCShaderCache::sharedShaderCache()->addProgram(program, kGAFStencilMaskAlphaFilterProgramCacheKey);
					program->release();
				}
				else
				{
					CCLOGERROR("Cannot load program for programShaderForMask.");
					return NULL;
				}
			}
			
			program->use();
			return program;
		}
	};
	
#if CC_ENABLE_CACHE_TEXTURE_DATA
	void _GAFreloadStencilShader()
	{
		CCGLProgram * program = CCShaderCache::sharedShaderCache()->programForKey(kGAFStencilMaskAlphaFilterProgramCacheKey);
		
		if (!program)
		{
			return;
		}
		program->reset();
		program = GAFShaderManager::createWithFragmentFilename(ccPositionTextureColor_vert, kPCStencilMaskAlphaFilterFragmentShaderFilename, program);
		if (program)
		{
			program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
			program->addAttribute(kCCAttributeNameColor,    kCCVertexAttrib_Color);
			program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
			program->link();
			program->updateUniforms();
			CHECK_GL_ERROR_DEBUG();
			CCLOGERROR("GAFStencilMaskSprite RELOADED");
		}
		else
		{
			CCAssert(false, "Can not RELOAD programShaderForMask");
		}
	}
	
	void _GAFreloadAlphaShader()
	{
		CCGLProgram * program = CCShaderCache::sharedShaderCache()->programForKey(kGAFSpriteWithAlphaShaderProgramCacheKey);
		
		if (!program)
		{
			return;
		}
		program->reset();
		program = GAFShaderManager::createWithFragmentFilename(ccPositionTextureColor_vert, kAlphaFragmentShaderFilename, program);
		if (program)
		{
			program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
			program->addAttribute(kCCAttributeNameColor,    kCCVertexAttrib_Color);
			program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
			program->link();
			program->updateUniforms();
			CHECK_GL_ERROR_DEBUG();
			program->use();
			_colorTrasformLocation = (GLuint)glGetUniformLocation(program->getProgram(), "colorTransform");
			if (_colorTrasformLocation <= 0)
			{
				CCAssert(false, "Can not RELOAD GAFSpriteWithAlpha");
			}
			CCLOGERROR("GAFSpriteWithAlpha RELOADED");
		}
		else
		{
			CCAssert(false, "Can not RELOAD GAFSpriteWithAlpha");
		}	
	}
#endif
	
	void * GAFMarmaladeGFX::createSprite(GAFSpriteCallback * cb)
	{
		GAFAssert(cb, "GAFSpriteCallback can not be null");
		return new GAFMarmaladeCocos2dxSprite(cb);
	}
	
	
	void * GAFMarmaladeGFX::createStencilSprite(GAFSpriteCallback * cb)
	{
		GAFAssert(cb, "GAFSpriteCallback can not be null");
		return new GAFMarmaladeCocos2dxStencilSprite(cb);
	}
	
	void GAFMarmaladeGFX::stencilSpriteAddMaskedObject(void * stencilSprite, void * maskedObject)
	{
		GAFMarmaladeCocos2dxStencilSprite * stencilSpr = (GAFMarmaladeCocos2dxStencilSprite *)stencilSprite;
		GAFMarmaladeCocos2dxSprite *        maskedObj = (GAFMarmaladeCocos2dxSprite *)maskedObject;
		stencilSpr->addMaskedObject(maskedObj);
	}
	
	void GAFMarmaladeGFX::stencilSpriteUpdateMaskContainerOf(void * stencilSprite)
	{
		GAFMarmaladeCocos2dxSprite * stencilSpr = (GAFMarmaladeCocos2dxSprite *)stencilSprite;
		GAFMarmaladeCocos2dxStencilSprite::updateMaskContainerOf(stencilSpr);
	}
	
	bool GAFMarmaladeGFX::initSprite(void * spr, void * texture, float x, float y, float w, float h, bool rotated)
	{
		if (!texture || !spr)
		{
			return false;
		}
		cocos2d::CCSprite * sprite = (cocos2d::CCSprite*) spr;
		cocos2d::CCTexture2D * tex = (cocos2d::CCTexture2D *) texture;
		return sprite->initWithTexture(tex, cocos2d::CCRect(x, y, w, h), rotated);
	}
	
	void GAFMarmaladeGFX::releaseSprite(void * sprite)
	{
		if (!sprite)
		{
			return;
		}
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		spr->release();
	}
	
	void * GAFMarmaladeGFX::createSpriteFrame(void * texture, float x, float y, float w, float h)
	{
		cocos2d::CCSpriteFrame * spriteFrame = new cocos2d::CCSpriteFrame();
		cocos2d::CCTexture2D * tex = (cocos2d::CCTexture2D *) texture;
		if (!spriteFrame->initWithTexture(tex, cocos2d::CCRect(x, y, w, h)))
		{
			delete spriteFrame;
			spriteFrame = NULL;
		}
		return spriteFrame;
	}
	
	void GAFMarmaladeGFX::releaseSpriteFrame(void * spriteFrame)
	{
		if (!spriteFrame)
		{
			return;
		}
		cocos2d::CCSpriteFrame * spr = (cocos2d::CCSpriteFrame *) spriteFrame;
		spr->release();
	}
	
	float GAFMarmaladeGFX::getSpriteContentSizeWidth(void * sprite)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->getContentSize().width;
	}
	
	float GAFMarmaladeGFX::getSpriteContentSizeHeight(void * sprite)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->getContentSize().height;
	}
	
	float GAFMarmaladeGFX::getSpriteAnchorPositionX(void * sprite)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->getAnchorPoint().x;
	}
	
	float GAFMarmaladeGFX::getSpriteAnchorPositionY(void * sprite)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->getAnchorPoint().y;
	}
	
	int  GAFMarmaladeGFX::getSpriteZorder(void * sprite)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->getZOrder();
	}
	
	void GAFMarmaladeGFX::setSpriteTransform(void * sprite, const CCAffineTransform& transform)
	{
		GAFMarmaladeCocos2dxSprite * spr = (GAFMarmaladeCocos2dxSprite *) sprite;
		spr->setTransform(transform);
	}
	
	void GAFMarmaladeGFX::setSpriteAtlasScale(void * sprite, float scale)
	{
		GAFMarmaladeCocos2dxSprite * spr = (GAFMarmaladeCocos2dxSprite *) sprite;		
		spr->setAtlasScale(scale);
	}
	
	void GAFMarmaladeGFX::setSpriteBlurRadius(void * sprite, float width, float height)
	{
		GAFMarmaladeCocos2dxSprite * spr = (GAFMarmaladeCocos2dxSprite *) sprite;
		spr->setBlurRadius(width, height);
	}
	
	void  GAFMarmaladeGFX::setSpriteVisible(void * sprite, bool visible)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->setVisible(visible);
	}
		
	void  GAFMarmaladeGFX::setSpriteShader(void * sprite, void * shader)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		cocos2d::CCGLProgram * program = (cocos2d::CCGLProgram *) shader;
		spr->setShaderProgram(program);
	}
	
	void GAFMarmaladeGFX::setSpriteAnchorPoint(void * sprite, float x, float y)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->setAnchorPoint(cocos2d::CCPoint(x, y));
	}
	
	void GAFMarmaladeGFX::setSpriteZorder(void * sprite, int zOrder)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->setZOrder(zOrder);
	}
	
	void GAFMarmaladeGFX::spriteContainerAddSprite(void * spriteContainer, void * sprite)
	{
		cocos2d::CCNode * cont = (cocos2d::CCNode *)spriteContainer;
		cocos2d::CCNode * spr = (cocos2d::CCNode *)sprite;
		cont->addChild(spr);		
	}
	
	void GAFMarmaladeGFX::spriteContainerRemoveSprite(void * spriteContainer, void * sprite, bool cleanup)
	{
		cocos2d::CCNode * spr = (cocos2d::CCNode *)sprite;
		spr->removeFromParentAndCleanup(true);
	}
	
	bool GAFMarmaladeGFX::initSpriteWithSpriteFrame(void * sprite, void * spriteFrame)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		cocos2d::CCSpriteFrame * sprFrame = (cocos2d::CCSpriteFrame *) spriteFrame;
		return spr->initWithSpriteFrame(sprFrame);
	}
	
	void * GAFMarmaladeGFX::getSpriteParent(void * sprite)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		return spr->getParent();
	}
	
	float GAFMarmaladeGFX::getAnimationInterval()
	{
		return roundf(1.0 / cocos2d::CCDirector::sharedDirector()->getAnimationInterval());
	}
	
	class UpdateAdapter : public cocos2d::CCObject
	{
	public:
		UpdateAdapter(GAF::CCObject * obj, GAFSEL_CallFunc sel)
		:
		_object(obj), _sel(sel)
		{
		}
		virtual void update(float dt)
		{
			if (_object && _sel)
			{
				(_object->*_sel)(dt);
			}
		}
	private:
		GAF::CCObject * _object;
		GAFSEL_CallFunc _sel;
	};
	
	void GAFMarmaladeGFX::scheduleUpdatePerFrame(GAF::CCObject * object, GAFSEL_CallFunc sel)
	{
		if (!object->userData())
		{
			UpdateAdapter * updater = new UpdateAdapter(object, sel);
			object->setUserData(updater);
			cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(updater, 0, false);
		}
	}
	
	void GAFMarmaladeGFX::unscheduleUpdatePerFrame(GAF::CCObject * object, GAFSEL_CallFunc sel)
	{
		if (object->userData())
		{
			UpdateAdapter * updater =  (UpdateAdapter *) object->userData();
			cocos2d::CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget(updater);
			object->setUserData(0);
			GAFAssert(updater->retainCount() == 1, "retain count must be 1");
			updater->release();
		}
	}

	void GAFMarmaladeGFX::setAlphaBlendingForSprite(void * sprite)
	{
		cocos2d::CCSprite * spr = (cocos2d::CCSprite *) sprite;
		cocos2d::ccBlendFunc bf;
		bf.src = GL_SRC_ALPHA;
		bf.dst = GL_ONE_MINUS_SRC_ALPHA;
		spr->setBlendFunc(bf);
	}
	
	void GAFMarmaladeGFX::setSpriteContainerZorder(void * spriteContainer, int zOrder)
	{
		cocos2d::CCNode * spr = (cocos2d::CCNode *) spriteContainer;
		spr->setZOrder(zOrder);
	}
	
	void GAFMarmaladeGFX::sceneAddSpriteContainer(void * scene, void * spriteContainer)
	{
		cocos2d::CCNode * sc  = (cocos2d::CCNode *) scene;
		cocos2d::CCNode * spr = (cocos2d::CCNode *) spriteContainer;
		sc->addChild(spr);
	}
	
	
	void  GAFMarmaladeGFX::sceneRemoveSpriteContainer(void * scene, void * spriteContainer)
	{
		cocos2d::CCNode * spr = (cocos2d::CCNode *) spriteContainer;
		spr->removeFromParentAndCleanup(true);
	}
	
	void GAFMarmaladeGFX::setSpriteContainerScale(void * spriteContaner, float scale)
	{
		cocos2d::CCNode * sc  = (cocos2d::CCNode *) spriteContaner;
		sc->setScale(scale);
	}
	
	void GAFMarmaladeGFX::setSpriteContainerPosition(void * sprteContainer, float x, float y)
	{
		cocos2d::CCNode * sc  = (cocos2d::CCNode *) sprteContainer;
		sc->setPosition(x, y);
	}
	
	void GAFMarmaladeGFX::setSpritePosition(void * sprte, float x, float y)
	{
		cocos2d::CCSprite * s  = (cocos2d::CCSprite *) sprte;
		s->setPosition(cocos2d::CCPoint(x, y));
	}
	
	void GAFMarmaladeGFX::setSpriteLocator(void * sprte, bool locator)
	{
		GAF::GAFCocos2dxSprite* s  = (GAF::GAFCocos2dxSprite *) sprte;
		s->setLocator(locator);
	}
	
	void GAFMarmaladeGFX::onFrame()
	{
		GAF::CCObject::process_autoreleased_objects();
	}
}
