#version 330 

in vec4 f_color;
in vec2 f_uv;
in float f_time;

out vec4 fragColor;

uniform sampler2D texture_map_text;
uniform float textX;
uniform float textY;
uniform vec4 textColor = vec4(1,0,0,1);

void main(void)
{
    vec2 uv = vec2((f_uv.x+textX)/8, (f_uv.y+textY)/8);

	// Retrieve texture value
    vec4 pixel = texture(texture_map_text, uv);

    if (pixel != vec4(0,0,0,1)) pixel *= textColor;
    else discard;

    // Use texture in determining fragment colour
    gl_FragColor = pixel;
}
