#version 330 core

uniform sampler2D inputTexture;

in vec2 texCoords;

out vec4 color;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

uniform float value;



// The MIT License
// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// https://www.youtube.com/c/InigoQuilez
// https://iquilezles.org


// Simplex Noise (http://en.wikipedia.org/wiki/Simplex_noise), a type of gradient noise
// that uses N+1 vertices for random gradient interpolation instead of 2^N as in regular
// latice based Gradient Noise.

// All noise functions here:
//
// https://www.shadertoy.com/playlist/fXlXzf&from=0&num=12



vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, vec3(70.0) );
}
const float PI = 3.14159265;

// from https://iquilezles.org/articles/distfunctions
float udRoundBox( vec2 p, vec2 b, float r )
{
    return length(max(abs(p)-b+r,0.0))-r;
}

float roundUp(vec2 uv) //! https://www.shadertoy.com/view/ldfSDj
{
    //TODO : Check if round corners effect texture rendering+
    // setup
    float t = 0.2 + 0.2 * sin(mod(0.75, 2.0 * PI) - 0.5 * PI);
    float iRadius = (0.05 + t)*1080;
    vec2 halfRes = vec2(0.5*1080);

    // compute box
    float b = udRoundBox( uv.xy*1080 - halfRes, halfRes, iRadius );
    
    // colorize (red / black )
	vec3 c = mix( vec3(1.0,0.0,0.0), vec3(0.0,0.0,0.0), smoothstep(0.0,1.0,b) );
        
    return c.r;
}


// -----------------------------------------------

void main( )
{
    float iresY = 0.5;
    vec2 ires = vec2(0.001,iresY);

    float time = 0.1;

    vec2 p = texCoords.xy / ires.xy;

	vec2 uv = p*vec2(ires.x/ires.y,1.0) + time*0.25;
	
	float f = 0.0;
	
    // left: value noise	
	if( p.x<0.6 )
	{
		f = noise( 16.0*uv );
	}
    // right: fractal noise (4 octaves)
    else	
	{
		uv *= 5.0;
        mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
		f  = 0.5000*noise( uv ); uv = m*uv;
		f += 0.2500*noise( uv ); uv = m*uv;
		f += 0.1250*noise( uv ); uv = m*uv;
		f += 0.0625*noise( uv ); uv = m*uv;
	}

	f = 0.5 + 0.5*f;
	
    f *= smoothstep( 0.0, 0.005, abs(p.x-0.6) );	
	
    vec3 rnd = vec3(rand(texCoords),rand(texCoords*0.1),rand(texCoords*0.2));
    rnd/=value*2;
    rnd+=1.0 - (1.0/value);

    vec3 txtr = texture(inputTexture,texCoords).rgb;
    f /= value;
    f += 1.0 - (1.0/value);
    
    txtr *= f;
    txtr *= rnd;

	color = vec4(txtr, 1.0 );
    if(roundUp(texCoords)<0.05)
        color.a = 0;
}

// void main()
// {
//     vec3 txtr = texture(inputTexture,texCoords).rgb;
    



//     txtr *= rnd;

//     txtr+=0.1;

//     color = vec4(txtr,1.0);   
// }