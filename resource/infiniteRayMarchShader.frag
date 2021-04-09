#version 330 

in vec4 f_color;
in vec2 f_uv;
in float f_time;

out vec4 fragColor;

uniform sampler2D texture_map;
uniform sampler2D second_texture_map;

uniform vec3 quadPos;
uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 world_mat;
uniform vec3 lightPos;

uniform vec3 ambient_color =  vec3(0.4,0.4,0.4);
uniform vec3 diffuse_color =  vec3(0.75,0.75,0.75);
uniform vec3 specular_color = vec3(0.8,0.8,0.8);
uniform float shine = 25.0f;
uniform float coefA =0.4;
uniform float coefD = 1;
uniform float coefS = 1.5;

vec3 rayMarch(vec3 start, vec3 ray);

vec3 SpherePos = vec3(0,0,0);


uniform vec3 camPos = vec3(0, 0, 4);
uniform vec3 cameraUp = vec3(0,1,0);
uniform vec3 cameraLookAt = vec3(0,0,0);

void main(void)
{

    vec2 pixelPosOnScreen = f_uv.xy*2 - 1.0;

    // Camera setup.
    vec3 direction=normalize(cameraLookAt-camPos);
    vec3 newUp =normalize(cross(cameraUp,direction));
    vec3 right =cross(direction,newUp);
    vec3 screenPos=(camPos+direction);
    //vec3 scrCoord=vcv+vPos.x*u*resolution.x/resolution.y+vPos.y*v;
    vec3 scrCoord=screenPos+pixelPosOnScreen.x*newUp*0.8+pixelPosOnScreen.y*right*0.8;
    vec3 ray=normalize(scrCoord-camPos);

    //Complete ray march for ray and return colour
    vec4 colReturned = vec4(rayMarch(camPos, ray),1);

    gl_FragColor = colReturned;
    
}

// --------------------------------------------------------------
const vec3 objectHitCol = vec3(0.9,0.8,0.9);
const float SphereRadius = 1;

float distanceToSphere(vec3 point) {
    return length(point - SpherePos) - SphereRadius;
}

float distanceToM(vec3 point) {
    vec3 z = point;
    float dr = 1;
    float r;

    float power = sin(f_time)*0.1 + 3;

    for (int i=0; i < 15; i++) {
        r = length(z);
        if(r > 2) break;

        float theta = acos(z.z/r) * power;
        float phi = atan(z.y,z.x) * power;
        float zr = pow(r, power);
        dr = pow(r, power-1) * power * dr + 1;

        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += point;
    }

    return 0.5* log(r) * r/dr;
}

float distanceToClosestObject(vec3 p) { 
    
    float c = 10;// sin(f_time)*5 + 10;
    vec3 point = mod(p+0.5*c, c) -0.5*c;
    return distanceToSphere(point);
}

const float dx = 0.0001;
const vec3 k = vec3(1,-1,0);
vec3 estimateNormal(vec3 point) {
    vec4 p = vec4(point, 1);
	return normalize(k.xyy*distanceToClosestObject((p + k.xyyz*dx).xyz) +
					 k.yyx*distanceToClosestObject((p + k.yyxz*dx).xyz) +
					 k.yxy*distanceToClosestObject((p + k.yxyz*dx).xyz) +
					 k.xxx*distanceToClosestObject((p + k.xxxz*dx).xyz));
}

vec3 calculateLighting(vec3 point) {

	vec3 N = estimateNormal(point);
	vec3 L = normalize(lightPos - point);
	vec3 V = normalize(camPos - point);
	vec3 R = normalize(reflect(-L, N));

    //the default color. 
	vec4 color  = vec4(objectHitCol,1.0);

	vec3 ambient, diffuse, specular;
	
	//ambient----------------------------------------
	ambient = coefA * ambient_color;

	//diffuse----------------------------------------
	diffuse = coefD * diffuse_color * max(dot(L, N), 0.f);
	
	//specular----------------------------------------
	specular = coefS * specular_color * pow(max(dot(R, V), 0.f), shine);

	color.rgb *= (ambient+diffuse+specular);
	return color.rgb;
	
}

const vec3 bgColor = vec3(0.53,0.81,0.92);
const float maxDistance = 1000.f;
const float minDistance = 0.0001f;
const int maxMarchingSteps = 100;

//Returns colour 
vec3 rayMarch(vec3 start, vec3 ray) {

    float currentDistance = 0.0f;
    float lastStep = currentDistance;
    float closestDistance = maxDistance;
    int curStep = 0;
    while (curStep < maxMarchingSteps) {

        vec3 currentPos = start + ray*currentDistance;
        float de = distanceToClosestObject(currentPos);
        
        closestDistance = min(abs(de), closestDistance); //Update closest recorded distance along march

        //Collision!
        if (de <= minDistance) {
            return calculateLighting(currentPos);
        }

        //Gone too far
        if (currentDistance >= maxDistance) {
            break;
        }

        currentDistance += de;
        curStep++;
    }

    //Ray did not hit a object
    
    vec3 glow = vec3(0.1,0,0.05);
    return bgColor;//mix(bgColor, glow, minDistance/closestDistance);
}
// --------------------------------------------------------------

