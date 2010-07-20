varying mediump vec4 qColor;
varying mediump vec4 qSecondaryColor;

void main(void)
{
    gl_FragColor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);
}
