////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gaussian blur
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uniform sampler2D inputImageTexture;

const lowp int GAUSSIAN_SAMPLES = 9;

varying highp vec2 textureCoordinate;
varying highp vec2 blurCoordinates[GAUSSIAN_SAMPLES];

void main()
{
 	lowp vec4 sum = vec4(0.0);
 	
    sum += texture2D(inputImageTexture, blurCoordinates[0]) * 0.05;
    sum += texture2D(inputImageTexture, blurCoordinates[1]) * 0.09;
    sum += texture2D(inputImageTexture, blurCoordinates[2]) * 0.12;
    sum += texture2D(inputImageTexture, blurCoordinates[3]) * 0.15;
    sum += texture2D(inputImageTexture, blurCoordinates[4]) * 0.18;
    sum += texture2D(inputImageTexture, blurCoordinates[5]) * 0.15;
    sum += texture2D(inputImageTexture, blurCoordinates[6]) * 0.12;
    sum += texture2D(inputImageTexture, blurCoordinates[7]) * 0.09;
    sum += texture2D(inputImageTexture, blurCoordinates[8]) * 0.05;
    
 	gl_FragColor = sum;
}

