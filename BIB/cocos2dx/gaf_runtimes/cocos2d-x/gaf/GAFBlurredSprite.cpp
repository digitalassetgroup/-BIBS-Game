#include "GAFBlurredSprite.h"
#include "GAFShaderManager.h"

#include "misc_nodes/CCRenderTexture.h"
#include "shaders/CCShaderCache.h"
#include "shaders/ccShaders.h"

static const char * kBlurredSpriteHorizontalBlurShaderFilename = "pcBlurredSprite_Shader_HorizontalBlur.fs";
static const char * kBlurredSpriteVerticalBlurShaderFilename =   "pcBlurredSprite_Shader_VerticalBlur.fs";



static int const kDefaultBlurSize = 7;
static int const kMaxGaussianMatrixSize = 51;
static int const kMaxBlurSize = 25; // kMaxBlurSize * 2 + 1 = kMaxGaussianMatrixSize

static bool isOdd (int number)
{
    return (number % 2) != 0;
}

static bool isEven (int number)
{
    return (number % 2) == 0;
}

static float gaussianValue (int x, float ro)
{
    return (exp(- (x * x) / (2 * ro * ro)) / sqrt(2 * M_PI * ro * ro));
}

static float * gaussianMatrixRow (int size, float ro)
{
    assert(size != 0);
    //assert((size - 1) % 2 == 0);
    
    float * result = (float*) malloc(sizeof(float) * size);
    int center = (size - 1) / 2;
    
    bool oddSize = ((size - 1) % 2 == 0);
    if (!oddSize)
    {
        size -= 1;
    }
    
    if (size == 1)
    {
        result[0] = 1.0;
    }
    else
    {
        for (int x = 0; (center - x) >= 0; x++)
        {
            result[center + x] = result[center - x] = gaussianValue(x, ro);
        }
    }
    
    // Workaround for even sizes
    if (!oddSize)
    {
        // actually result's size == size + 1
        result[size] = 0;
    }
    
    return result;
}

#if CC_ENABLE_CACHE_TEXTURE_DATA
#include <set>
static std::set<GAFBlurredSprite *> _blurredSprites;
#endif

GAFBlurredSprite::GAFBlurredSprite()
:
_gaussianValuesX(NULL),
_gaussianValuesY(NULL),
_dynamicTexture1(NULL),
_dynamicTexture2(NULL),
_originalTexture(NULL),
_sourceSprite(NULL)
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
	_blurredSprites.insert(this);
#endif
}

GAFBlurredSprite::~GAFBlurredSprite()
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
	_blurredSprites.erase(this);
#endif
	if (_gaussianValuesX)
	{
		free(_gaussianValuesX);
	}
	if (_gaussianValuesY)
	{
		free(_gaussianValuesY);
	}
	CC_SAFE_RELEASE(_dynamicTexture1);
	CC_SAFE_RELEASE(_dynamicTexture2);
	CC_SAFE_RELEASE(_originalTexture);
	CC_SAFE_RELEASE(_sourceSprite);
}

void GAFBlurredSprite::prepareDynamicsForSize(CCSize aSize)
{
	if (!_dynamicTexture1 || _blurredTextureSize.width != aSize.width || _blurredTextureSize.height != aSize.height)
    {
		CC_SAFE_RELEASE(_dynamicTexture1);
		_dynamicTexture1 = CCRenderTexture::create(aSize.width, aSize.height);
		CC_SAFE_RETAIN(_dynamicTexture1);
    }	
	if (!_dynamicTexture2 || _blurredTextureSize.width != aSize.width || _blurredTextureSize.height != aSize.height)
    {
		CC_SAFE_RELEASE(_dynamicTexture2);
		_dynamicTexture2 = CCRenderTexture::create(aSize.width, aSize.height);
		CC_SAFE_RETAIN(_dynamicTexture2);
    }
    _blurredTextureSize = aSize;
}

bool GAFBlurredSprite::initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated, CCSize aBlurSize)
{
	if (!GAFSpriteWithAlpha::initWithTexture(pTexture, rect, rotated))
	{
		return false;
	}
	
	_originalTextureSize = rect.size;
	_blurSize = normalizedBlurSizeFromNeededBlurSize(aBlurSize);
	createGaussianMatrixWithSize(_blurSize);
	CCSize blurredSize = blurTextureSizeForOriginalSize(_originalTextureSize);
	
	prepareDynamicsForSize(blurredSize);

	CC_SAFE_RELEASE(_originalTexture);
	_originalTexture = CCRenderTexture::create(blurredSize.width, blurredSize.height);
	CC_SAFE_RETAIN(_originalTexture);

	_originalTexture->beginWithClear(1, 1, 1, 0);
	
	GAFSprite * dynamicSprite = new GAFSprite();
	dynamicSprite->initWithTexture(pTexture, rect, rotated);
	dynamicSprite->setAnchorPoint(CCPoint(0.5, 0.5));
	dynamicSprite->setPosition(CCPointMake(blurredSize.width / 2.0, blurredSize.height / 2.0));
	dynamicSprite->visit();
	dynamicSprite->release();
	_originalTexture->end();

	return true;
}

bool GAFBlurredSprite::initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated)
{
	return initWithTexture(pTexture, rect, rotated, CCSizeMake(kDefaultBlurSize, kDefaultBlurSize));
}

bool GAFBlurredSprite::initWithGAFSprite(GAFSpriteWithAlpha * aSprite, CCSize aBlurSize)
{
	if (!initWithTexture(aSprite->getTexture(), aSprite->getTextureRect(), aSprite->isTextureRectRotated(), aBlurSize))
	{
		return false;
	}
	
	CC_SAFE_RELEASE(_sourceSprite);
	_sourceSprite = aSprite;
	CC_SAFE_RETAIN(_sourceSprite);

	setColorTransform(aSprite->getColorTransform());
	setUseExternalTransform(aSprite->isUseExternalTransform());
	setAnchorPoint(aSprite->getAnchorPoint());
	setExternaTransform(aSprite->getExternalTransform());
	setVisible(aSprite->isVisible());
	setZOrder(aSprite->getZOrder());
	objectId = aSprite->objectId;
	setVertexZ(aSprite->getVertexZ());
	setBlendFunc(aSprite->getBlendFunc());
	return true;
}

CCSize GAFBlurredSprite::normalizedBlurSizeFromNeededBlurSize(CCSize aSize)
{
	CCSize result = CCSizeMake(MIN(aSize.width, kMaxGaussianMatrixSize),
                               MIN(aSize.height, kMaxGaussianMatrixSize));
    
    result.width = result.width + (isOdd((int)result.width) ? 0 : 1);
    result.height = result.height + (isOdd((int)result.height) ? 0 : 1);
    return result;
}

void GAFBlurredSprite::createGaussianMatrixWithSize(CCSize aSize)
{
    CCAssert(aSize.width > 0 && aSize.height > 0, "");
    CCAssert(isOdd((int)aSize.width) && isOdd((int)aSize.height), "");
    
    const float roCoeff = 3;
    
    if (aSize.width != _gaussianValuesArraySize.width)
    {
        if (_gaussianValuesX)
		{
			free(_gaussianValuesX);
		}        			
		float ro = (aSize.width / 2) / roCoeff;
		_gaussianValuesX = gaussianMatrixRow(aSize.width, ro);
	}
    
    if (aSize.height != _gaussianValuesArraySize.height)
    {
        if (_gaussianValuesY)
		{
			free(_gaussianValuesY);
		}
		float ro = (aSize.height / 2) / roCoeff;
		_gaussianValuesY = gaussianMatrixRow(aSize.height, ro);
	}
	
    _gaussianValuesArraySize = aSize;
}

CCSize GAFBlurredSprite::blurTextureSizeForOriginalSize(CCSize anOriginalSize)
{
    return CCSizeMake(anOriginalSize.width + (_blurSize.width - 1) * 2,
                      anOriginalSize.height + (_blurSize.height - 1) * 2);
}

void GAFBlurredSprite::setBlurSize(CCSize blurSize)
{
	bool adjusted = false;
    if (blurSize.width > kMaxBlurSize)
    {
        blurSize.width = kMaxBlurSize;
        adjusted = true;
    }
    if (blurSize.height > kMaxBlurSize)
    {
        blurSize.height = kMaxBlurSize;
        adjusted = true;
    }
    if (adjusted)
    {
        CCLOGINFO("blur size should not be greater than %d. Using maximum value instead.", kMaxBlurSize);
    }
    
    if (_blurSize.width != blurSize.width || _blurSize.height != blurSize.height)
    {
		
        _blurSize = normalizedBlurSizeFromNeededBlurSize(blurSize);
		createGaussianMatrixWithSize(CCSizeMake((_blurSize.width * 2) + 1,
												(_blurSize.height * 2) + 1));
    }
}

CCGLProgram * GAFBlurredSprite::programForVerticalBlur()
{
	
	CCGLProgram *program = CCShaderCache::sharedShaderCache()->programForKey(kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey);

    if (!program)
    {
        program = GAFShaderManager::createWithFragmentFilename(ccPositionTextureColor_vert, kBlurredSpriteVerticalBlurShaderFilename);        
        if (program)
        {
			program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
			program->addAttribute(kCCAttributeNameColor,    kCCVertexAttrib_Color);
			program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            program->link();
			program->updateUniforms();
            CHECK_GL_ERROR_DEBUG();
			CCShaderCache::sharedShaderCache()->addProgram(program, kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey);
            program->release();
        }
        else
        {
            CCLOGERROR("Cannot load program for kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey.");
            return NULL;
        }
    }
	
    program->use();
    
    _vertShader_kernelSizeUniformLocation   = (GLint)glGetUniformLocation(program->getProgram(), "u_matrixRowSize");
    _vertShader_blurDotSizeUniformLocation  = (GLint)glGetUniformLocation(program->getProgram(), "dotSize");
	// had to change parameter name, looks like a Apple bug
    _vertShader_kernelValuesUniformLocation = (GLint)glGetUniformLocation(program->getProgram(), "u_matrixRowValues");
    
    if (_vertShader_kernelSizeUniformLocation <= 0 ||
        _vertShader_blurDotSizeUniformLocation <= 0 ||
        _vertShader_kernelValuesUniformLocation <= 0)
    {
        CCLOGERROR("Cannot get uniforms for kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey\n%s\n",  program->fragmentShaderLog());
    }
    
    return program;
}

CCGLProgram * GAFBlurredSprite::programForHorizontalBlur()
{
	CCGLProgram *program = CCShaderCache::sharedShaderCache()->programForKey(kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey);
	
    if (!program)
    {
        program = GAFShaderManager::createWithFragmentFilename(ccPositionTextureColor_vert, kBlurredSpriteHorizontalBlurShaderFilename);
        if (program)
        {
			program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
			program->addAttribute(kCCAttributeNameColor,    kCCVertexAttrib_Color);
			program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            program->link();
			program->updateUniforms();
            CHECK_GL_ERROR_DEBUG();
			CCShaderCache::sharedShaderCache()->addProgram(program, kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey);
            program->release();
        }
        else
        {
            CCLOGERROR("Cannot load program for kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey.");
            return NULL;
        }
    }
	
	program->use();
    
    _horzShader_kernelSizeUniformLocation   = (GLint)glGetUniformLocation(program->getProgram(), "u_matrixRowSize");
    _horzShader_blurDotSizeUniformLocation  = (GLint)glGetUniformLocation(program->getProgram(), "dotSize"); // had to change parameter name, looks like a Apple bug
    _horzShader_kernelValuesUniformLocation = (GLint)glGetUniformLocation(program->getProgram(), "u_matrixRowValues");
    
    if (_horzShader_kernelSizeUniformLocation <= 0 ||
        _horzShader_blurDotSizeUniformLocation <= 0 ||
        _horzShader_kernelValuesUniformLocation <= 0)
    {
        CCLOGERROR("Cannot get uniforms for kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey");
    }
    
    return program;
}

void GAFBlurredSprite::draw()
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
	if (!CCShaderCache::sharedShaderCache()->programForKey(kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey) ||
		!CCShaderCache::sharedShaderCache()->programForKey(kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey))
	{
		std::set<GAFBlurredSprite*>::iterator it = _blurredSprites.begin();
		while (it != _blurredSprites.end())
		{
			if ((*it) != this)
			{
				(*it)->updateDynamicTexture();
			}
			++it;
		}
		updateDynamicTexture();
	}
#endif
	GAFSpriteWithAlpha::draw();
}

void GAFBlurredSprite::updateDynamicTexture()
{
	// Preparing dynamic textures
    CCSize blurredSize = blurTextureSizeForOriginalSize(_originalTextureSize);
    prepareDynamicsForSize(blurredSize);
	
    /* We will use two-pass gaussian filter, first horizontal, then vertical.
     * 1. Render 'original' texture to 'dynamicTexture1' using horizontal blur pixel shader
     * 2. Render 'dynamicTexture1' to 'dynamicTexture2' using vertical blur pixel shader
     * 3. Use 'dynamicTexture2' as texture for our sprite
     */
    
    // Caching shaders


    CCGLProgram *program1 = programForHorizontalBlur();
    CCGLProgram *program2 = programForVerticalBlur();
    if (!program1 || !program2)
    {
        CCLOGERROR("Failed to compile shaders for GAFBlurredSprite. Will display incorrectly.");
        return;
    }
    
    CHECK_GL_ERROR_DEBUG();
 
    //////////
    // Step 0 - render original to tex2
    _dynamicTexture2->beginWithClear(1.0, 1.0, 1.0, 0.0);
    
	CCPoint pt = CCPointMake(_dynamicTexture1->getSprite()->getContentSize().width / 2,
							 _dynamicTexture1->getSprite()->getContentSize().height / 2);
    // Draw sprite
    _originalTexture->getSprite()->setPosition(pt);
    
    _originalTexture->getSprite()->setBlendFunc((ccBlendFunc){ GL_ONE, GL_ZERO });
    _originalTexture->getSprite()->visit();
    _dynamicTexture2->end();
	

    //////////
    // Step 1 - render tex2 to tex1. Use shader.
    _dynamicTexture1->beginWithClear(1.0, 1.0, 1.0, 0.0);
    
    // Set shader to sprite
    _dynamicTexture2->getSprite()->setShaderProgram(programForHorizontalBlur());
    
    CHECK_GL_ERROR_DEBUG();
    // Set uniform values
    glUniform1i(_horzShader_kernelSizeUniformLocation, _gaussianValuesArraySize.width);
    glUniform1f(_horzShader_blurDotSizeUniformLocation, (1.0 / _blurredTextureSize.width));
    glUniform1fv(_horzShader_kernelValuesUniformLocation, _gaussianValuesArraySize.width, (GLfloat *)_gaussianValuesX);
    
    // Draw sprite
    _dynamicTexture2->getSprite()->setPosition(pt);
    
    _dynamicTexture2->getSprite()->setBlendFunc((ccBlendFunc){ GL_ONE, GL_ZERO });
    _dynamicTexture2->getSprite()->visit();
    
    _dynamicTexture1->end();
 	   
    //////////
    // Step 2 - render tex1 to tex2. Use shader.
    _dynamicTexture2->beginWithClear(1.0, 1.0, 1.0, 0.0);
    
    // Set shader to sprite
    _dynamicTexture1->getSprite()->setShaderProgram(programForVerticalBlur());
    
    CHECK_GL_ERROR_DEBUG();
    // Set uniform values
    glUniform1i(_vertShader_kernelSizeUniformLocation, _gaussianValuesArraySize.height);
    glUniform1f(_vertShader_blurDotSizeUniformLocation, (1.0 / _blurredTextureSize.height));
    glUniform1fv(_vertShader_kernelValuesUniformLocation, _gaussianValuesArraySize.height, (GLfloat *)_gaussianValuesY);
    
    // Draw sprite
    _dynamicTexture1->getSprite()->setPosition(pt);	
    _dynamicTexture1->getSprite()->setBlendFunc((ccBlendFunc){ GL_ONE, GL_ZERO });
    _dynamicTexture1->getSprite()->visit();
    
    _dynamicTexture2->end();

    //////////
    // Step 3
    
    //self.texture = _originalTexture.sprite.texture;
    //self.texture = _dynamicTexture1.sprite.texture;
	setTexture(_dynamicTexture2->getSprite()->getTexture());
    setFlipY(true);
    
    // Adjust this sprite's data
    CCRect textureRect = CCRectMake(0, 0, _blurredTextureSize.width, _blurredTextureSize.height);
	setTextureRect(textureRect, isTextureRectRotated(), textureRect.size);
    
    // Adjust pivot point due to texture size change (making new object positioned at the same place as old)
    CCPoint pivotPoint = _sourceSprite->getAnchorPoint();
    CCPoint scale = CCPointMake(_blurredTextureSize.width / _sourceSprite->getContentSize().width,
                                _blurredTextureSize.height / _sourceSprite->getContentSize().height);
    pivotPoint.x = 0.5 - ((0.5 - pivotPoint.x) / scale.x);
    pivotPoint.y = 0.5 - ((0.5 - pivotPoint.y) / scale.y);
    
	setAnchorPoint(pivotPoint);
}
