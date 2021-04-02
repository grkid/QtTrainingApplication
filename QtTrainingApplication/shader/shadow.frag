#version 330 core
out vec2 color;
void main()
{
//    float depth = linearizeDepth(gl_FragCoord.z);
//    depth = (depth - nearPlane)/(farPlane - nearPlane);
//    FragColor.x = depth;
//    FragColor.y = depth*depth;
	float depth=gl_FragCoord.z;
	depth=depth*0.5+0.5;
   color=vec2(depth,depth*depth);
} 