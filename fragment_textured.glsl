
uniform sampler2D diffuse;
uniform int greyscaleFlag;
varying vec2 texCoordVar;

void main() {
    gl_FragColor = texture2D(diffuse, texCoordVar);
    if (greyscaleFlag == 0){
        gl_FragColor = texture2D(diffuse, texCoordVar);
    }
    else{
        vec4 color = texture2D(diffuse, texCoordVar);
        vec3 luminance = vec3(dot(vec3(0.2126, 0.7152, 0.0722), color.rgb));
        gl_FragColor = vec4(luminance, color.a);
        
        
    }
}
