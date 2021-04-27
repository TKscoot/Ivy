#version 450 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;
out vec3 pos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	//TexCoords = aPos;
	//gl_Position = projection * view * vec4(aPos, 1.0);
	//vec4 pos = projection * view * vec4(aPos, 1.0);
	//gl_Position = pos.xyww;

    vec4 clip_pos = vec4(aPos.xy, -1.0, 1.0);
    vec4 view_pos  = inverse(projection*view) * clip_pos;
   
    vec3 dir = vec3(view_pos);//vec3(inv_view * vec4(view_pos.x, view_pos.y, -1.0, 0.0));
    dir = normalize(dir);

    TexCoords = dir;
	

    gl_Position = vec4(aPos.xyz, 1.0f);
	
	//pos = view_pos.rgb;
	pos = transpose(mat3(view)) * (inverse(projection) * vec4(aPos, 1.0)).xyz;
}
