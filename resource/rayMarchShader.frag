#version 330 

in vec4 f_color;
in vec2 f_uv;
in float f_time;

out vec4 fragColor;

uniform vec3 lightPos;

uniform int sceneID = 0;
uniform vec3 objectHitCol = vec3(1,0.9,0.63);
uniform vec3 bgColor = vec3(0.53,0.81,0.92);
uniform vec3 glow = vec3(1,1,1);
uniform bool SHADOWS_ENABLED = true;
uniform bool GLOW_ENABLED = true;
uniform bool FOG_ENABLED = true;
uniform bool AMBIENT_OCCLUSION_ENABLED = true;
uniform float maxDistance = 1000.f;
uniform float minFogDistance = 25.f; //Distance where fog starts setting in
uniform int glowSteps = 5; //Min marching steps where glow is applied

uniform vec3 ambient_color =  vec3(0.4,0.4,0.4);
uniform vec3 diffuse_color =  vec3(0.75,0.75,0.75);
uniform vec3 specular_color = vec3(0.8,0.8,0.8);
uniform float shine = 25.0f;
uniform float coefA =0.6;
uniform float coefD = 1;
uniform float coefS = 1.5;

uniform float testvarA;

vec3 rayMarch(vec3 start, vec3 ray);

vec3 SpherePos = vec3(0,0,0);

uniform vec3 camPos = vec3(0, 0, 4);
uniform vec3 cameraUp = vec3(0,1,0);
uniform vec3 cameraLookAt = vec3(0,0,0);

vec3 ray;

void main(void)
{
    vec2 pixelPosOnScreen = f_uv.xy*2 - 1.0;

    // Camera setup.
    vec3 direction=normalize(cameraLookAt-camPos);
    vec3 newUp =normalize(cross(cameraUp,direction));
    vec3 right =cross(direction,newUp);
    vec3 screenPos=(camPos+direction);
    vec3 scrCoord=screenPos+pixelPosOnScreen.x*newUp*0.8+pixelPosOnScreen.y*right*0.8;
    ray=normalize(scrCoord-camPos);

    //Complete ray march for ray and return colour
    vec4 colReturned = vec4(rayMarch(camPos, ray),1);

    gl_FragColor = colReturned;
    
}

// --------------------------------------------------------------
//OBJECT MANIPULATION HELPER FUNCTIONS
float intersect(float distA, float distB) {
    return max(distA, distB);
}

float union(float distA, float distB) {
    return min(distA, distB);
}

float difference(float distA, float distB) {
    return max(distA, -distB);
}

float smoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h); 
}

float smoothSubtraction( float d1, float d2, float k ) {
    float h = clamp( 0.5 - 0.5*(d2+d1)/k, 0.0, 1.0 );
    return mix( d2, -d1, h ) + k*h*(1.0-h); 
}

float smoothIntersection( float d1, float d2, float k ) {
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

const float SphereRadius = 1;

float distanceToSphere(vec3 point) {
    return length(point - SpherePos) - SphereRadius;
}

float distanceToCube(vec3 point, vec3 volume) {
    return length(max(abs(point)-volume,0.0));
}

float distanceToSquare( in vec2 p, in vec2 b )
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

vec3 sierpinskiFold(vec3 p) {
    vec3 z = p;
	z.xy -= min(z.x + z.y, 0.0);
	z.xz -= min(z.x + z.z, 0.0);
	z.yz -= min(z.y + z.z, 0.0);
    return z;
}

float distanceToSierpiensky(vec3 p) {
    
    float s = 200;
    vec3 s1 = sierpinskiFold(p);
    for (int i = 0; i < int(testvarA); i++) {
        s1 = translate(s1,vec3(-s,-s,-s));
        s1 = sierpinskiFold(s1);
        s/=2;
    }
    float d = distanceToTetrahedron(translate(s1,vec3(-s,-s,-s)), s);

    return d;
    
}

float distanceToCross(vec3 p) {
    float da = distanceToSquare(p.xy,vec2(1.0));
    float db = distanceToSquare(p.yz,vec2(1.0));
    float dc = distanceToSquare(p.zx,vec2(1.0));
    return union(da,union(db,dc));
}

float distanceToMenger(vec3 p) {
    float d = distanceToCube(p,vec3(1.0));

   float s = sin(f_time)*1.5 + 3;
   for( int m=0; m<int(testvarA)+2; m++ )
   {
      vec3 a = mod( p*s, 2.0 )-1.0;
      s *= 3.0;
      vec3 r = 1.0 - 3.0*abs(a);

      float c = distanceToCross(r)/s;
      d = intersect(d,c);
   }

   return d;
}

float distanceToBlob(vec3 p) {
    float d = sin(4.5 * p.x * sin(f_time)) * sin(4.5 * p.y* sin(f_time)) * sin(4.5 * p.z* -sin(f_time)) * 0.25;
    float orbDist = sin(f_time)*2 + 1;
    float df = smoothUnion(distanceToSphere(p + d), distanceToSphere((translate(p, vec3(sin(f_time),0,cos(f_time))*orbDist) - d)/0.25)*0.25, 0.5);
    df = smoothUnion(df, distanceToSphere((translate(p, vec3(cos(f_time),0,sin(f_time))*orbDist) - d)/0.25)*0.25, 0.5);
    df = smoothUnion(df, distanceToSphere((translate(p, vec3(cos(f_time),sin(f_time),sin(f_time))*orbDist) - d)/0.25)*0.25, 0.5);
    df = smoothUnion(df, distanceToSphere((translate(p, vec3(cos(f_time),cos(f_time),sin(f_time))*orbDist) - d)/0.25)*0.25, 0.5);
    df = smoothUnion(df, distanceToSphere((translate(p, vec3(cos(f_time),-cos(f_time),-sin(f_time))*orbDist) - d)/0.25)*0.25, 0.5);
    orbDist = -sin(f_time)*0.5 + 1;
    df = smoothUnion(df, distanceToSphere((translate(p, vec3(1,1,-1)*orbDist) - d)/0.25)*0.25, 0.5);
    df = smoothUnion(df, distanceToSphere((translate(p, vec3(1,-1,1)*orbDist) - d)/0.25)*0.25, 0.5);
    df = smoothUnion(df, distanceToSphere((translate(p, vec3(0,1,0)*orbDist) - d)/0.25)*0.25, 0.5);
    df = smoothUnion(df, distanceToSphere((translate(p, vec3(-1,1,1)*orbDist) - d)/0.25)*0.25, 0.5);
    return df;
}


float distanceToClosestObject(vec3 p) {    
    
    if (sceneID == 0) {
        vec3 point = mod(p+0.5*10, 10) -0.5*10; //Repeat object every 10 units
        return distanceToSphere(point);
    }
    if (sceneID == 1) return distanceToSierpiensky(p);
    if (sceneID == 2) return distanceToMenger(translate(p, vec3(0,-100,0))/100)*100; //Scale object by 100 and shift down 100 units
    if (sceneID == 3) return distanceToM(p);
    if (sceneID == 4) return distanceToBlob(p);

}

const float dx = 0.001;
const float backStep = 0.01f;
const vec3 k = vec3(1,-1,0);
vec3 estimateNormal(vec3 point) {
    vec4 p = vec4(point-ray*backStep, 1);//Step back a little before getting normal to fix small lighting issues
	return normalize(k.xyy*distanceToClosestObject((p + k.xyyz*dx).xyz) +
					 k.yyx*distanceToClosestObject((p + k.yyxz*dx).xyz) +
					 k.yxy*distanceToClosestObject((p + k.yxyz*dx).xyz) +
					 k.xxx*distanceToClosestObject((p + k.xxxz*dx).xyz));
}

const int maxMarchingSteps = 200;
vec2 lightRayMarch(vec3 start, vec3 ray);

vec3 calculateLighting(vec3 point) {

	vec3 N = estimateNormal(point);
	vec3 L = normalize(lightPos - point);
	vec3 V = normalize(camPos - point);
	vec3 R = normalize(reflect(-L, N));

    //the default color. 
	vec4 color  = vec4(objectHitCol,1.0);

	vec3 ambient, diffuse, specular;
	
    bool inShadow = false;
    vec2 lightMarch;
    if (SHADOWS_ENABLED) {
        lightMarch = lightRayMarch(point, L);
        inShadow = lightMarch.x == 1 ? true : false;
    }

	//ambient----------------------------------------
	if (SHADOWS_ENABLED && AMBIENT_OCCLUSION_ENABLED) {
        float occlusionP = 1 - (lightMarch.y / maxMarchingSteps);
        ambient = coefA * ambient_color * occlusionP;
    }
    else ambient = coefA * ambient_color;

	//diffuse----------------------------------------
    if (!inShadow) diffuse = coefD * diffuse_color * max(dot(L, N), 0.f);
	else diffuse = vec3(0);

	//specular----------------------------------------
	if (!inShadow) specular = coefS * specular_color * pow(max(dot(R, V), 0.f), shine);
    else specular = vec3(0);

	color.rgb *= (ambient+diffuse+specular);
	return color.rgb;
	
}

vec3 calculateBlobLighting(vec3 point) {

	vec3 N = estimateNormal(point);
	vec3 L = normalize(lightPos - point);
	vec3 V = normalize(camPos - point);
	vec3 R = normalize(reflect(-L, N));

    //the default color. 
	vec4 color  = vec4(normalize(-point),1);

	vec3 ambient, diffuse, specular;
	
    bool inShadow = false;
    vec2 lightMarch;
    if (SHADOWS_ENABLED) {
        lightMarch = lightRayMarch(point, L);
        inShadow = lightMarch.x == 1 ? true : false;
    }

	//ambient----------------------------------------
	if (SHADOWS_ENABLED && AMBIENT_OCCLUSION_ENABLED) {
        float occlusionP = 1 - (lightMarch.y / maxMarchingSteps);
        ambient = coefA * ambient_color * occlusionP;
    }
    else ambient = coefA * ambient_color;

	//diffuse----------------------------------------
    if (!inShadow) diffuse = coefD * diffuse_color * max(dot(L, N), 0.f);
	else diffuse = vec3(0);

	//specular----------------------------------------
	if (!inShadow) specular = coefS * specular_color * pow(max(dot(R, V), 0.f), shine);
    else specular = vec3(0);

	color.rgb *= (ambient+diffuse+specular);
	return color.rgb;
	
}


const float minDistance = 0.001f;

vec3 addGlowCol(vec3 col, int curStep) {
    if (curStep < glowSteps) return col;
    else {
        float glowPercentage = float(curStep-glowSteps)/float(maxMarchingSteps-glowSteps);
        return mix(col, glow, glowPercentage);
    }
}

vec3 addFogCol(vec3 col, float currentDistance) {
    if (currentDistance < minFogDistance) return col;
    float fogPercentage = (currentDistance-minFogDistance)/(maxDistance-minFogDistance);
    return mix(col, bgColor, fogPercentage); 
}

vec2 lightRayMarch(vec3 start, vec3 ray) {
     float currentDistance = 0.1f;
     int curStep = 0;
     while (curStep < maxMarchingSteps) {

        vec3 currentPos = start + ray*currentDistance;
        float de = distanceToClosestObject(currentPos);
        
        //Collision!
        if (de <= minDistance) {

            return vec2(1, curStep);
        }

        //Gone too far
        if (currentDistance >= maxDistance) {
            return vec2(0, curStep);
        }

        currentDistance += de;
        curStep++;
    }

    return vec2(0, curStep);
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

            vec3 col = (sceneID == 4) ? calculateBlobLighting(currentPos) : calculateLighting(currentPos);

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
    return col;
}
// --------------------------------------------------------------

