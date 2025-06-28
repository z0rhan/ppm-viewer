#shader vertex
#version 460
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPosition, 0.0, 1.0);
    TexCoords = aTexCoords;
}

#shader fragment
#version 460
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D imageTexture;

void main()
{
    FragColor = texture(imageTexture, TexCoords);
}
