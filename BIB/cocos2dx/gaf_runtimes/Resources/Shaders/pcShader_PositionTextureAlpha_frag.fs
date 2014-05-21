////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Simple shader with added color transform
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef GL_ES
precision lowp float;
#endif

varying vec2 v_texCoord;
uniform vec4 colorTransform[2]; // mult + offsets
uniform sampler2D u_texture;

void main()
{
    //gl_FragColor = texture2D(u_texture, v_texCoord);
    vec4 texColor = texture2D(u_texture, v_texCoord);
	gl_FragColor = texColor * colorTransform[0] + colorTransform[1];
}


