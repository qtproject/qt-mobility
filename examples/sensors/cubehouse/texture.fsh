uniform sampler2D tex;
varying mediump vec4 qColor;
varying mediump vec4 qSecondaryColor;
varying highp vec4 qTexCoord;

void main(void)
{
    mediump vec4 col = texture2D(tex, qTexCoord.st);
    mediump vec4 lcolor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);
    gl_FragColor = vec4(clamp(lcolor.rgb * (1.0 - col.a) + col.rgb * col.a, 0.0, 1.0), lcolor.a);
}
