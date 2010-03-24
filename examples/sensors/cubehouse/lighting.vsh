// Algorithm from section 2.14.1 of OpenGL 2.1 specification.

attribute highp vec4 vertex;
attribute highp vec3 normal;
attribute highp vec4 texcoord;
uniform mediump mat4 matrix;
uniform mediump mat4 modelView;
uniform mediump mat3 normalMatrix;
varying highp vec4 qTexCoord;

uniform mediump vec4 acli;      // Ambient intensity of the light
uniform mediump vec4 dcli;      // Diffuse intensity of the light
uniform mediump vec4 scli;      // Specular intensity of the light
uniform mediump vec3 sdli;      // Direction of the light
uniform mediump vec3 pli;       // Position of the light
uniform mediump float pliw;     // 0 for directional, 1 for positional.
uniform mediump float srli;     // Spotlight exponent for the light
uniform mediump float crli;     // Spotlight cutoff for the light
uniform mediump float ccrli;    // Cosine of spotlight cutoff for the light
uniform mediump float k0;       // Constant attentuation factor for the light
uniform mediump float k1;       // Linear attentuation factor for the light
uniform mediump float k2;       // Quadratic attentuation factor for the light
uniform mediump vec4 acm[2];    // Ambient color of the material
uniform mediump vec4 dcm[2];    // Diffuse color of the material
uniform mediump vec4 scm[2];    // Specular color of the material
uniform mediump vec4 ecm[2];    // Emissive color of the material
uniform mediump float srm[2];   // Specular exponent of the material
uniform mediump vec4 acs;       // Light model's ambient color of the scene
uniform bool viewerAtInfinity;  // Light model indicates viewer at infinity
uniform bool twoSided;          // Light model indicates two-sided lighting

varying mediump vec4 qColor;
varying mediump vec4 qSecondaryColor;

void qLightVertex(vec4 vertex, vec3 normal)
{
    int i, material;
    vec3 toEye, toLight, h;
    float angle, spot, attenuation;
    vec4 color, scolor;
    vec4 adcomponent, scomponent;

    // Determine which material to use.
    if (!twoSided || normal.z >= 0.0) {
        material = 0;
    } else {
        material = 1;
        normal = -normal;
    }

    // Start with the material's emissive color and the ambient scene color.
    color = ecm[material] + acm[material] * acs;
    scolor = vec4(0, 0, 0, 0);

    // Vector from the vertex to the eye position (i.e. the origin).
    if (viewerAtInfinity)
        toEye = vec3(0, 0, 1);
    else
        toEye = normalize(-vertex.xyz);

    // Determine the cosine of the angle between the normal and the
    // vector from the vertex to the light.
    if (pliw == 0.0)
        toLight = normalize(pli);
    else
        toLight = normalize(pli - vertex.xyz);
    angle = max(dot(normal, toLight), 0.0);

    // Calculate the ambient and diffuse light components.
    adcomponent = acm[material] * acli + angle * dcm[material] * dcli;

    // Calculate the specular light components.
    if (angle != 0.0) {
        h = normalize(toLight + toEye);
        angle = max(dot(normal, h), 0.0);
        scomponent = pow(angle, srm[material]) * scm[material] * scli;
    } else {
        scomponent = vec4(0, 0, 0, 0);
    }

    // Apply the spotlight angle and exponent.
    if (crli != 180.0) {
        spot = max(dot(normalize(vertex.xyz - pli), normalize(sdli)), 0.0);
        if (spot < ccrli)
            spot = 0.0;
        else
            spot = pow(spot, srli);
    } else {
        spot = 1.0;
    }
    adcomponent *= spot;
    scomponent *= spot;

    // Apply attenuation to the colors.
    if (pliw != 0.0) {
        attenuation = k0;
        if (k1 != 0.0 || k2 != 0.0) {
            float len = length(pli - vertex.xyz);
            attenuation += k1 * len + k2 * len * len;
        }
        color += adcomponent / attenuation;
        scolor += scomponent / attenuation;
    } else {
        color += adcomponent;
        scolor += scomponent;
    }

    // Generate the final output colors.
    float alpha = dcm[material].a;
    qColor = vec4(clamp(color.rgb, 0.0, 1.0), alpha);
    qSecondaryColor = clamp(scolor, 0.0, 1.0);
}

void main(void)
{
    gl_Position = matrix * vertex;
    highp vec4 vertex = modelView * vertex;
    highp vec3 norm = normalize(normalMatrix * normal);
    qLightVertex(vertex, norm);
    qTexCoord = texcoord;
}
