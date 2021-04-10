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
uniform float coefA =0.6;
uniform float coefD = 1;
uniform float coefS = 1.5;

uniform float testvarA;

vec3 rayMarch(vec3 start, vec3 ray);

vec3 SpherePos = vec3(-1,3,0);


uniform vec3 camPos = vec3(0, 0, 4);
uniform vec3 cameraUp = vec3(0,1,0);
uniform vec3 cameraLookAt = vec3(0,0,0);

vec3 ray;

void main(void)
{
    SpherePos = vec3(sin(f_time) + 1, -3, 0);
    vec2 pixelPosOnScreen = f_uv.xy*2 - 1.0;

    // Camera setup.
    vec3 direction=normalize(cameraLookAt-camPos);
    vec3 newUp =normalize(cross(cameraUp,direction));
    vec3 right =cross(direction,newUp);
    vec3 screenPos=(camPos+direction);
    //vec3 scrCoord=vcv+vPos.x*u*resolution.x/resolution.y+vPos.y*v;
    vec3 scrCoord=screenPos+pixelPosOnScreen.x*newUp*0.8+pixelPosOnScreen.y*right*0.8;
    ray=normalize(scrCoord-camPos);

    //Complete ray march for ray and return colour
    vec4 colReturned = vec4(rayMarch(camPos, ray),1);

    gl_FragColor = colReturned;
    
}

// --------------------------------------------------------------
//MATH HELPER FUNCTIONS
float intersect(float distA, float distB) {
    return max(distA, distB);
}

float union(float distA, float distB) {
    return min(distA, distB);
}

float difference(float distA, float distB) {
    return max(distA, -distB);
}

float opSmoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h); 
}

float opSmoothSubtraction( float d1, float d2, float k ) {
    float h = clamp( 0.5 - 0.5*(d2+d1)/k, 0.0, 1.0 );
    return mix( d2, -d1, h ) + k*h*(1.0-h); 
}

float opSmoothIntersection( float d1, float d2, float k ) {
    float h = clamp( 0.5 - 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) + k*h*(1.0-h); 
}

vec3 translate(vec3 point, vec3 trans)
{
    return point + trans;
}

vec3 fold(vec3 p, vec3 n, float d) {
    return p-2*n*min(0,dot(p,n)-d);
}

// --------------------------------------------------------------

const vec3 objectHitCol = vec3(1,0.9,0.63);
const float SphereRadius = 1;

float distanceToSphere(vec3 point) {
    return length(point - SpherePos) - SphereRadius;
}

float distanceToCube(vec3 point, vec3 volume) {
    return length(max(abs(point)-volume,0.0));
}

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

const float floorY = 10f;
float distanceToFloor(vec3 point) {
    return abs(point.y);
}

float distanceToM(vec3 point) {
    vec3 z = point;
    float dr = 1;
    float r;

    float power = f_time*0.1 + 1;

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

float distanceToTetrahedron(vec3 p, float r) {
    float md = max(max(-p.x - p.y - p.z, p.x + p.y - p.z),
				   max(-p.x + p.y + p.z, p.x - p.y + p.z));
	return (md - r) / (sqrt(3.0));
}

float distanceToSierpiensky(vec3 point) {
    
    float s = 1;
    vec3 p = point;

    float pws = 1;

    int iterations = 5;
    for (int i=0; i < iterations; i++) {

        p = fold(p, normalize(vec3(1,1,0)), 0);
        p = translate(p, vec3(0, 2, 0));

        p = fold(p, normalize(vec3(0,1,1)), 0);
        p = translate(p, vec3(-2, -2, 0));

        p = fold(p, normalize(vec3(1,0,1)), 0);
        p = translate(p, vec3(-2, 0, -2));

        p = fold(p, normalize(vec3(1,0,1)), 0);
        p = fold(p, normalize(vec3(1,1,0)), 0);
        p = fold(p, normalize(vec3(0,1,1)), 0);
        p = translate(p, vec3(-pws, -pws, -pws));

    }
    
    return distanceToTetrahedron(p, s);
    
}

const float inf = 10000f;

float distanceToCross(vec3 p) {
    float da = sdBox(p.xy,vec2(1.0));
    float db = sdBox(p.yz,vec2(1.0));
    float dc = sdBox(p.zx,vec2(1.0));
    return min(da,min(db,dc));
}

float distanceToMenger(vec3 p) {
    float d = distanceToCube(p,vec3(1.0));

   float s = 1.0;
   for( int m=0; m<5; m++ )
   {
      vec3 a = mod( p*s, 2.0 )-1.0;
      s *= 3.0;
      vec3 r = 1.0 - 3.0*abs(a);

      float c = distanceToCross(r)/s;
      d = max(d,c);
   }

   return d;
}

float distanceToClosestObject(vec3 p) {
    return union(distanceToMenger(p + vec3(0,3,0)), distanceToFloor(p));
}

const float dx = 0.001;
const float backStep = 0.01f;
const vec3 k = vec3(1,-1,0);
vec3 estimateNormal(vec3 point) {
    vec4 p = vec4(point-ray*backStep, 1);
	return normalize(k.xyy*distanceToClosestObject((p + k.xyyz*dx).xyz) +
					 k.yyx*distanceToClosestObject((p + k.yyxz*dx).xyz) +
					 k.yxy*distanceToClosestObject((p + k.yxyz*dx).xyz) +
					 k.xxx*distanceToClosestObject((p + k.xxxz*dx).xyz));
}

const bool SHADOWS_ENABLED = true;

bool lightRayMarch(vec3 start, vec3 ray);

vec3 calculateLighting(vec3 point) {

	vec3 N = estimateNormal(point);
	vec3 L = normalize(lightPos - point);
	vec3 V = normalize(camPos - point);
	vec3 R = normalize(reflect(-L, N));

    //the default color. 
	vec4 color  = vec4(objectHitCol,1.0);

	vec3 ambient, diffuse, specular;
	
    bool inShadow = false;
    if (SHADOWS_ENABLED) inShadow = lightRayMarch(point, L);

	//ambient----------------------------------------
	ambient = coefA * ambient_color;

	//diffuse----------------------------------------
    if (!inShadow) diffuse = coefD * diffuse_color * max(dot(L, N), 0.f);
	else diffuse = vec3(0);

	//specular----------------------------------------
	if (!inShadow) specular = coefS * specular_color * pow(max(dot(R, V), 0.f), shine);
    else specular = vec3(0);

	color.rgb *= (ambient+diffuse+specular);
	return color.rgb;
	
}

const bool GLOW_ENABLED = true;
const bool FOG_ENABLED = true;

const vec3 bgColor = vec3(0.53,0.81,0.92);
const vec3 glow = vec3(1,1,1);
const float maxDistance = 1000.f;
const float minDistance = 0.001f;
const float fogDistance = 25.f; //Distance where fog starts setting in
const int glowSteps = 10; //Min marching steps where glow is applied
const int maxMarchingSteps = 250;

vec3 addGlowCol(vec3 col, int curStep) {
    if (curStep < glowSteps) return col;
    else {
        float glowPercentage = float(curStep-glowSteps)/float(maxMarchingSteps-glowSteps);
        return mix(col, glow, glowPercentage);
    }
}

vec3 addFogCol(vec3 col, float currentDistance) {
    if (currentDistance < fogDistance) return col;
    float fogPercentage = (currentDistance-fogDistance)/(maxDistance-fogDistance);
    return mix(col, bgColor, fogPercentage); 
}

bool lightRayMarch(vec3 start, vec3 ray) {
     float currentDistance = 0.1f;
     int curStep = 0;
     while (curStep < maxMarchingSteps) {

        vec3 currentPos = start + ray*currentDistance;
        float de = distanceToClosestObject(currentPos);
        
        //Collision!
        if (de <= minDistance) {

            return true;
        }

        //Gone too far
        if (currentDistance >= maxDistance) {
            return false;
        }

        currentDistance += de;
        curStep++;
    }

    return false;
}

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

            vec3 col = calculateLighting(currentPos);

            //Apply effects
            if (GLOW_ENABLED) col = addGlowCol(col, curStep);
            if (FOG_ENABLED) col = addFogCol(col, currentDistance);

            return col;
        }

        //Gone too far
        if (currentDistance >= maxDistance) {
            break;
        }

        currentDistance += de;
        curStep++;
    }

    //Debug
    //if (curStep == maxMarchingSteps) return vec3(1,0,0);

    //Ray did not hit a object
    vec3 col = bgColor;
    if (GLOW_ENABLED) col = addGlowCol(col, curStep);
    //if (FOG_ENABLED) col = addFogCol(col, currentDistance);
    return col;
}
// --------------------------------------------------------------

