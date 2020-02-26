float4x4 g_mCameraViewProj;
float4x4 g_mObjWorld;

texture g_texVideo; sampler2D g_samVideo = sampler_state  { Texture = <g_texVideo>;  MinFilter = Linear;  MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };


sampler2D g_samMirrorVideo = sampler_state  { Texture = <g_texVideo>;  MinFilter = Linear;  MagFilter = Linear; AddressU = Mirror;   AddressV = Mirror; };


texture g_texSecond; sampler2D g_samSecond = sampler_state  { Texture = <g_texSecond>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };


texture g_texBlur0;  sampler2D g_samBlur0 = sampler_state  { Texture = <g_texBlur0>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur1;  sampler2D g_samBlur1 = sampler_state  { Texture = <g_texBlur1>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur2;  sampler2D g_samBlur2 = sampler_state  { Texture = <g_texBlur2>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur3;  sampler2D g_samBlur3 = sampler_state  { Texture = <g_texBlur3>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur4;  sampler2D g_samBlur4 = sampler_state  { Texture = <g_texBlur4>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur5;  sampler2D g_samBlur5 = sampler_state  { Texture = <g_texBlur5>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur6;  sampler2D g_samBlur6 = sampler_state  { Texture = <g_texBlur6>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur7;  sampler2D g_samBlur7 = sampler_state  { Texture = <g_texBlur7>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur8;  sampler2D g_samBlur8 = sampler_state  { Texture = <g_texBlur8>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur9;  sampler2D g_samBlur9 = sampler_state  { Texture = <g_texBlur9>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur10; sampler2D g_samBlur10 = sampler_state { Texture = <g_texBlur10>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur11; sampler2D g_samBlur11 = sampler_state { Texture = <g_texBlur11>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur12; sampler2D g_samBlur12 = sampler_state { Texture = <g_texBlur12>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur13; sampler2D g_samBlur13 = sampler_state { Texture = <g_texBlur13>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur14; sampler2D g_samBlur14 = sampler_state { Texture = <g_texBlur14>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };
texture g_texBlur15; sampler2D g_samBlur15 = sampler_state { Texture = <g_texBlur15>; MinFilter = Linear; MagFilter = Linear; AddressU = Wrap;   AddressV = Wrap; };




float g_blend=0.0f;
float g_multiTex=1.0f;
float g_videoAsp=1.0f;
float g_fLevel=0.0f;
float g_level=0.0f;
float g_dest_bright=0;
float g_time = 0.0f;
float4 g_eyepos;

float g_video_w = 1280;
float g_video_h = 720;
const float PI = 3.141592658;
/////////////////////////////////////////////////////////////////////////////////
#define vec2 float2
#define vec4 float4
#define vec3 float3

#define mix lerp

struct VS_INPUT
{
    float4 iPos 	: POSITION;
	float4 Diffuse  : COLOR0;
	float2 iTex 	: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos		: POSITION;
    float4 Color 	: COLOR0;
    float2 Tex 		: TEXCOORD0;
};

VS_OUTPUT vs_common( const VS_INPUT i )
{
    VS_OUTPUT o = (VS_OUTPUT)0;
    
	float4 f = mul( float4(i.iPos.xyz, 1), g_mObjWorld );//這不能互換
	
    o.Pos = mul( f, g_mCameraViewProj );
    o.Color = i.Diffuse;
    o.Tex = i.iTex;
    return o;
}

//////////////////////////////////////////////////////////////////

float4 ps_negative( VS_OUTPUT i ) : COLOR0
{
	 vec4 color =  tex2D( g_samVideo, i.Tex );
	 vec4 dest = vec4((1.0 - color.rgb), 1);
     return mix(color, dest, g_level );
}

technique tech_negative
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_negative();          
    }
}
  
/////////////////////////////////////////////////////////////////////////////////
uniform float g_factor;
const vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

float4 ps_gray( VS_OUTPUT i ) : COLOR0
{
	 vec3 color =  tex2D( g_samVideo, i.Tex ).rgb;
	 float gray = dot(color.rgb, luminanceWeighting);
	 float dest = g_factor*(gray-0.5)+0.5;
	 dest=clamp(dest,0.0,1.0);
	 vec3 ok = mix(  color.rgb, vec3(dest,dest,dest), g_level );
	 return vec4(ok.r, ok.g, ok.b, 1);
}

technique tech_gray
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_gray();          
    }
}
  
  
/////////////////////////////////////////////////////////////////////////////////
float g_r, g_g,  g_b;

float4 ps_rgbColor( VS_OUTPUT i ) : COLOR0
{
	vec3 c =  tex2D( g_samVideo, i.Tex ).rgb;
	vec3 dest = vec3( c.r+g_r,  c.g+g_g, c.b+g_b);
	vec3 ok = mix(c.rgb, dest, g_level );
	return vec4(ok.r, ok.g, ok.b, 1);
}

technique tech_rgbColor
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_rgbColor();          
    }
}
  

/////////////////////////////////////////////////////////////////////////////////
float g_minx;
float g_maxx;
float g_miny;
float g_maxy;

float4 ps_rectbrightdark( VS_OUTPUT i,   float2 vPos : VPOS ) : COLOR0
{
	vec4 c =  tex2D( g_samVideo, i.Tex );
	vec3 dest = vec3(0.0, 0.0, 0.0);
		
	if(vPos.x>g_minx  && vPos.x<g_maxx && vPos.y>g_miny && vPos.y<g_maxy )
		  dest = vec3( c.r+g_dest_bright,  c.g+g_dest_bright, c.b+g_dest_bright);

	else  dest = vec3( c.r-g_dest_bright,  c.g-g_dest_bright, c.b-g_dest_bright);

	dest = clamp(dest, 0.0, 1.0);
	return mix(c,vec4(dest,1), g_level);
}

technique tech_rectbrightdark
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_rectbrightdark();          
    }
}



/////////////////////////////////////////////////////////////////////////////////


float4 ps_brightflicker( VS_OUTPUT i ) : COLOR0
{
	vec3 c =  tex2D( g_samVideo, i.Tex ).rgb;
	vec3 dest = vec3( c.r+g_dest_bright,  c.g+g_dest_bright, c.b+g_dest_bright);
	dest = clamp(dest, 0.0, 1.0);
	vec3 q = mix( c, dest, g_fLevel);
	
	return vec4(q.r, q.g, q.b, 1.0);
}

technique tech_brightflicker
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_brightflicker();          
    }
}
  
/////////////////////////////////////////////////////////////////////////////////

float4 ps_color( VS_OUTPUT i ) : COLOR0
{
	 float4 ok =  tex2D( g_samVideo, i.Tex );
	 return ok;
	 //return float4( ok.r*i.Color.r,  ok.g*i.Color.g, ok.b*i.Color.b, 1.0f);
}


technique tech_color
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_color();          
    }
}

int modi(int x, int y){	return x%y;}
float modF(float x, float y){	return x - y*floor(x/y);}

//////////////////////////////////////////////////////////////////////////////
uniform int g_offset;

float4 ps_crosshatching(VS_OUTPUT i, float2 vPos : VPOS) : COLOR0
{
	 const float lum_threshold_1 = 0.4*0.4;
	 const float lum_threshold_2 = 0.8*0.8;

	 vec4 c = tex2D(g_samVideo, i.Tex);
	 float  tc = 1.0;

	 float lum = c.r*c.r + c.g*c.g + c.b*c.b;
	 if (lum < lum_threshold_1){ if (modi(vPos.x + vPos.y, g_offset) == 0.0)tc=0.0; }
	 if (lum < lum_threshold_2){ if (modi(vPos.x - vPos.y, g_offset) == 0.0)tc=0.0; }
	 return  lerp( c, vec4(tc,tc,tc, 1), g_level);
}			
 
technique tech_crosshatching
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_crosshatching();          
    }
}

/////////////////////////////////////////////////////////////////////////////////
uniform float g_rotate=0;
uniform float g_sx=1;
uniform float g_sy=1;
uniform float g_ox=0;
uniform float g_oy=0;
			
vec3 rotateZ(float rad, vec3 p)
{ 	float c = cos(rad);  float s = sin(rad); float3x3 R = float3x3( c, s, 0.0, -s, c, 0.0,  0.0, 0.0, 1.0 ); 
	return mul(R,p); //跟opengles  相反
}
vec3 scale2D(float x, float y, vec3 p){   p.x*=x; p.y*=y;  return p; }
vec3 offsetXY(float x, float y, vec3 p){  p.x+=x; p.y+=y;  return p; }
			
VS_OUTPUT vs_rotatescale( const VS_INPUT i )
{
    VS_OUTPUT o = (VS_OUTPUT)0;
    
	 vec3 a = scale2D(g_sx, g_sy, i.iPos.xyz);
	 a = rotateZ(g_rotate, a);
	 a = offsetXY(g_ox, g_oy, a);

	float4 f = float4(a.xyz, 1);
    o.Pos = mul( f, g_mCameraViewProj );
    o.Color = i.Diffuse;
    o.Tex = i.iTex;
    return o;
}
  
technique tech_rotatescale
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_rotatescale();
        PixelShader  = compile ps_3_0 ps_color();          
    }
}


/////////////////////////////////////////////////////////////////////////////////
float g_viewport_w;
float g_viewport_h;
			
float4 ps_pixelate(VS_OUTPUT i) : COLOR0
{
	 vec2 uv =  i.Tex;
	 float dx = g_level*(1.0/g_viewport_w);
	 float dy = g_level*(1.0/g_viewport_h);
	 vec2 coord = vec2(dx*floor(uv.x/dx),  dy*floor(uv.y/dy));
	 return tex2D(g_samVideo, coord);
}
  
technique tech_pixelate
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_pixelate();          
    }
}

/////////////////////////////////////////////////////////////////////////////////
 
const float2 swirl_center=float2(0.5,0.5f);
float swirl_radius=0.5f;
float swirl_amount=0;
 
float4 ps_swirl(VS_OUTPUT i) : COLOR0
{
	float2 coords = i.Tex;
	coords = coords - swirl_center;
	float r = length(coords);
	
	if(r <= swirl_radius)
	{
			float phi = atan2(coords.y, coords.x);
			float distortion = pow(swirl_amount * ((swirl_radius - r) / swirl_radius), 2);
		   
			if(swirl_amount >= 0)phi = phi + distortion;
			else				 phi = phi - distortion;
		   
			coords.x = r * cos(phi);
			coords.y = r * sin(phi);
	}
	return tex2D(g_samVideo, coords + swirl_center);
}
  
technique tech_swirl
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_swirl();          
    }
}

/////////////////////////////////////////////////////////////////////////////////
float g_bright=0.0f;
 
float4 ps_blind(VS_OUTPUT i) : COLOR0
{
	float2 secTex = i.Tex*g_multiTex;
	secTex.x *=g_videoAsp; //還跟視窗比例有關
	float4 sec  = tex2D(g_samSecond, secTex);
	float4 video= tex2D(g_samVideo,  i.Tex)*(1+g_bright);
	
	float blend = g_blend*sec.a;
	float3 ok = blend * sec.rgb + (1-blend)*video.rgb;
	return float4( ok, 1);
}
 
  
technique tech_blind
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_blind();          
    }
}

/////////////////////////////////////////////////////////////////////////////////
float g_neno_offx=0.0;
float g_neno_offy=0.0;
 
float4 ps_neon(VS_OUTPUT i) : COLOR0
{															//跟opengles不一樣	
	float2 secTex = float2(g_neno_offx*cos(g_time), g_neno_offy*-sin(g_time)) + i.Tex*g_multiTex;
		
	secTex.x *= g_videoAsp; //還跟視窗比例有關
	float4 video = tex2D(g_samVideo,  i.Tex);
	float4 sec   = tex2D(g_samSecond, secTex);
	float blend = g_blend*sec.a;
	float3 ok = blend * sec.rgb + (1.0-blend)*video.rgb;
	return float4( ok, 1);
}
 
  
technique tech_neon
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_neon();          
    }
}




/////////////////////////////////////////////////////////////////////////////////


float __min_channel(float3 v)
{
    float t = (v.x<v.y) ? v.x : v.y;
    t = (t<v.z) ? t : v.z;
    return t;
}

float __max_channel(float3 v)
{
    float t = (v.x>v.y) ? v.x : v.y;
    t = (t>v.z) ? t : v.z;
    return t;
}

//h:[0,360], s,l:[0,100]
float3 rgb_to_hsl(float3 ok)
{
    float r = ok.r, g=ok.g,  b=ok.b;
	float h,s,l;
	
	float M = __max_channel(ok);
	float m = __min_channel(ok);
	float d = M-m;
	if( d==0 ) h=0;
	else if( M==r ) h=((g-b)/d)%6;
	else if( M==g ) h=(b-r)/d+2;
	else h=(r-g)/d+4;
	
	h*=60;
	if( h<0 ) h+=360;
	l = (M+m)/2;
	
	if( d==0 )	s = 0;
	else		s = d/(1-abs(2*l-1));
	s*=100;
	l*=100;
	
	return float3(h,s,l);
}


float3 hsl_to_rgb(float3 HSV)
{
	float h=HSV.r, s=HSV.g, l=HSV.b;

	if( h<0 ) h+=360;
	if( s<0 ) s=0;
	if( l<0 ) l=0;
	if( h>=360 ) h=359;
	if( s>100 ) s=100;
	if( l>100 ) l=100;
	s/=100;
	l/=100;
	float C = (1-abs(2*l-1))*s;
	float hh = h/60;
	float X = C*(1-abs(hh%2-1));
	float r = 0, g =0,  b = 0;
	
	     if( hh>=0 && hh<1 ){	r = C;	g = X;	}
	else if( hh>=1 && hh<2 ){	r = X;	g = C;	}
	else if( hh>=2 && hh<3 ){	g = C;	b = X;	}
	else if( hh>=3 && hh<4 ){	g = X;	b = C;	}
	else if( hh>=4 && hh<5 ){	r = X;	b = C;	}
	else				    {	r = C;	b = X;	}
	
	float m = l-C/2;
	r += m;
	g += m;
	b += m;
			
	return float3(r,g,b);
}



float  g_h=0.0f, g_s=0.0f, g_l=0.0f;

float4 ps_hsl_to_rgb(VS_OUTPUT i) : COLOR0
{
	float4 c = tex2D(g_samVideo,  i.Tex);
	
	
	float3 hsl = rgb_to_hsl( c.rgb );
	
	 hsl.r += g_h;  //Hue
	 hsl.g += g_s;  //Saturation
	 hsl.b += g_l;  //Lightness

	float3 ok = hsl_to_rgb(hsl);
	
	float3 q = lerp( c.rgb, ok, g_fLevel);
	return float4( q.rgb, 1);
}
 
  
technique tech_hsl_to_rgb
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_hsl_to_rgb();          
    }
}
 

/////////////////////////////////////////////////////////////////////////////////
 

float4 ps_window(VS_OUTPUT i) : COLOR0
{
	float2 secTex = i.Tex*g_multiTex;
	secTex.x *=g_videoAsp; //還跟視窗比例有關
	
	float3 video = tex2D(g_samVideo,  i.Tex).rgb;
	
	float2 disp = 0.05*( tex2D(g_samSecond,secTex).xy );
	float4 ok = tex2D(g_samVideo, i.Tex+disp);
	
	return ok;
}
 
  
technique tech_window
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_window();          
    }
}

/////////////////////////////////////////////////////////////////////////////////


float g_ripplelen,g_rippletime, g_ripplescale;
float g_ripplex=0, g_rippley=0;

 
float4 ps_ripple(VS_OUTPUT i) : COLOR0
{
	float2 tc = i.Tex;
	float2 p = -1.0 + 2.0*tc+float2(g_ripplex,g_rippley);
	float len = length(p);
	float2 uv = tc + (p/len)*cos(len*g_ripplelen-g_time*g_rippletime)*g_ripplescale;
	float3 col = tex2D(g_samVideo, uv).rgb;
	
	return float4(col,1);  
}
  
technique tech_ripple
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_ripple();          
    }
}


/////////////////////////////////////////////////////////////////////////////////
float g_fisheye=1, g_fisheyeD=0.4; //
 
float4 ps_fisheye(VS_OUTPUT i) : COLOR0
{
	 float2 p = i.Tex;

	 float2 m = float2(0.5, 0.5);
	 float2 d = p - m;
	 float r = length(d);
	 float bind= length(m);
	 
	 float power =  PI/bind*g_fisheyeD;

	 float2 uv = m + normalize(d)*tan(r*power)/ (tan(bind*power*g_fisheye));
	
	 return  tex2D(g_samVideo, float2(uv.x, -uv.y) );
}
 
  
technique tech_fisheye
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_fisheye();          
    }
}


/////////////////////////////////////////////////////////////////////////////////

float fun_gray(float3 srcPixel )
{
	return dot( float3( 0.2125, 0.7154, 0.0721), srcPixel );
}

float g_halftone=1;

float added(vec2 sh,   float d)
{
	return 0.5 + 0.25 * cos(sh.y*d) + 0.25 * cos(sh.x*d);
}

float4 ps_halftone(VS_OUTPUT i, float2 vPos : VPOS ) : COLOR0
{
	float threshold = 0.9; 
	
	float2 dstCoord = float2(vPos.x/1800, vPos.y/1800);
		
	float dotSize = g_halftone;
	
	float rasterPattern = added( dstCoord, PI/dotSize*680.0);
	float3 srcPixel = tex2D(g_samVideo, i.Tex).rgb;
	float avg = dot( float3( 0.2125, 0.7154, 0.0721), srcPixel );
	
    float gray = (rasterPattern*threshold + avg - threshold) / (1.0 - threshold);
  
    float3 cc = float3(gray, gray, gray);
    float3 hsl = rgb_to_hsl( cc );
	hsl.b += g_l;  //Lightness
	float3 ok = hsl_to_rgb(hsl);
	
	
	return float4(ok, 1.0);
}
 
  
technique tech_halftone
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_halftone();          
    }
}


////////////////////////////////////////////////////////////////////
 
float random (in float2 st) 
{ 
    return frac(sin(dot(st.xy,  float2(12.9898,78.233)))  * 43758.5453123);
}

 
float noise2(float2 st) 
{
    float2 i = floor(st);
    float2 f = frac(st);
    float2 u = f*f*(3.0-2.0*f);
    return lerp( lerp( random( i + float2(0.0,0.0) ),   random( i + float2(1.0,0.0) ), u.x),
                lerp( random( i + float2(0.0,1.0) ),    random( i + float2(1.0,1.0) ), u.x), u.y);
}
 

float2 rotate2d(float angle, float2 pos)
{
	float c=cos(angle), s=sin(angle);
    float2x2  rot = float2x2(c,-s, s,c);
	return mul( pos, rot);
}

float lines(in float2 pos, float b){
    float scale = 10.0;
    pos *= scale;
    return smoothstep(0.0, .5+b*.5,  abs((sin(pos.x*3.1415)+b*2.0))*.5);
}

float g_woodx=3, g_woody=1.5f;
float g_woodR=0,g_woodG=0,g_woodB=0;

float4 ps_woodnoise(VS_OUTPUT i) : COLOR0
{
    float2 pos=i.Tex*float2(g_woodx,g_woody);

    float pattern = pos.x;
    pos = rotate2d( noise2(pos*g_time), pos ) ;
	
    pattern = lines(pos,.5);

	float4  ok =  tex2D( g_samVideo, i.Tex );
	float4 c = float4(g_woodR, g_woodG, g_woodB,1 );
	return lerp( c, ok, pattern);     
}


technique tech_woodnoise
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_woodnoise();          
    }
}


/////////////////////////////////////////////////////////////////////////////////
float3x3 scharrKernelX = float3x3(3.0, 10.0, 3.0,
                                  0.0, 0.0, 0.0,
                                -3.0, -10.0, -3.0);

float3x3 scharrKernelY = float3x3( 3.0, 0.0, -3.0,
                                  10.0, 0.0, -10.0,
                                  3.0, 0.0, -3.0);

 

float convolve(float3x3 kernel, float3x3 image) 
{
    float result = 0.0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result += kernel[i][j]*image[i][j];
        }
    }
    return result;
}


float convolveComponent(float3x3 kernelX, float3x3 kernelY, float3x3 image) 
{
    float2 result;
    result.x = convolve(kernelX, image);
    result.y = convolve(kernelY, image);
    return clamp(length(result), 0.0, 255.0);
}

 

float4 edge(float stepx, float stepy, float2 center, float3x3 kernelX, float3x3 kernelY)
{
    float3x3 image = float3x3(
					  length(tex2D(g_samVideo,center + float2(-stepx,stepy)).rgb),
                      length(tex2D(g_samVideo,center + float2(0,stepy)).rgb),
                      length(tex2D(g_samVideo,center + float2(stepx,stepy)).rgb),
                      length(tex2D(g_samVideo,center + float2(-stepx,0)).rgb),
                      length(tex2D(g_samVideo,center).rgb),
                      length(tex2D(g_samVideo,center + float2(stepx,0)).rgb),
                      length(tex2D(g_samVideo,center + float2(-stepx,-stepy)).rgb),
                      length(tex2D(g_samVideo,center + float2(0,-stepy)).rgb),
                      length(tex2D(g_samVideo,center + float2(stepx,-stepy)).rgb));
    float2 result;
    result.x = convolve(kernelX, image);
    result.y = convolve(kernelY, image);
    
    float c = clamp(length(result), 0.0, 255.0);
    return float4(c,c,c,1);
}

float4 trueColorEdge(float stepx, float stepy, float2 center, float3x3 kernelX, float3x3 kernelY) 
{
    float4 edgeVal = edge(stepx, stepy, center, kernelX, kernelY);
    return edgeVal * tex2D(g_samVideo,center);
}
							   
float g_edgewidth=1;
float4 ps_edge(VS_OUTPUT i,  float2 vPos : VPOS  ) : COLOR0
{
	float2 sketchSize=float2(g_video_w, g_video_h );

	float STEP=g_edgewidth*6.0f;
	return trueColorEdge( STEP/sketchSize.x, STEP/sketchSize.y,   i.Tex,  scharrKernelX, scharrKernelY );
	
}
 
  
technique tech_edge
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_edge();          
    }
}


///////////////////////////////////////////////////////////////////////////////////////

float g_waveFreq=8.5, g_waveNum=16, g_waveTwist=0.01;

float wave(vec2 pos, float t, float freq, float numWaves, vec2 center) 
{
	float d = length(pos - center);
	d = log(5.0 + exp(d));
	return 1.0/(1.0+20.0*d*d) * sin(2.0*3.14159*(-numWaves*d + t*freq));
}
 
float height(vec2 pos, float t) 
{
	float w;
	w =  wave(pos, t, g_waveFreq, g_waveNum,  vec2(0.5, -0.5));
	w += wave(pos, t, g_waveFreq, g_waveNum, -vec2(0.5, -0.5));
	return w;
}


float4 ps_wave(VS_OUTPUT i) : COLOR0
{
	vec2 uv = i.Tex; 
	vec2 uvn = 2.0*uv - vec2(1,1);	
	
	uv.x += height(uvn - vec2(g_waveTwist, g_waveTwist), g_time) - height(uvn, g_time);
	uv.y += height(uvn - vec2(g_waveTwist, g_waveTwist), g_time) - height(uvn, g_time);
	
	return  tex2D(g_samVideo, vec2(uv.x, uv.y));
}


technique tech_wave
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_wave();          
    }
}


/////////////////////////////////////////////////////////////////////////////////////
float g_colorpaint=6;
float4 ps_watercolor(VS_OUTPUT I) : COLOR0
{
	vec2 src_size = vec2 (g_colorpaint/g_video_w, g_colorpaint/g_video_h);
    vec2 uv =  I.Tex;

     float4 m0=0,m1=0,m2=0,m3=0;
     float4 s0=0,s1=0,s2=0,s3=0;
     float4 c;
	 int i,j,a=0;

	 //
	 const int radius = 2; //數字太大 會變很慢

	 for(j = -radius; j <= 0; ++j)for(i = -radius; i <= 0; ++i){ c = tex2D(g_samVideo, uv + vec2(i,j) * src_size);   m0 += c;   s0 += c*c;  }    
     for(j = -radius; j <= 0; ++j)for(i = 0; i <= radius;  ++i){ c = tex2D(g_samVideo, uv + vec2(i,j) * src_size);   m1 += c;   s1 += c*c;  }    
     for(j = 0; j <= radius;  ++j)for(i = 0; i <= radius;  ++i){ c = tex2D(g_samVideo, uv + vec2(i,j) * src_size);   m2 += c;   s2 += c*c;  }   
     for(j = 0; j <= radius;  ++j)for(i = -radius; i <= 0; ++i){ c = tex2D(g_samVideo, uv + vec2(i,j) * src_size);   m3 += c;   s3 += c*c;  }   

	 
	 float4 fragColor=0;
     float sigma2, min_sigma2 = 1e+2;
	 float n = float( (radius+1) * (radius+1) );
	 
     m0 /= n; s0 = abs(s0/n - m0*m0); sigma2 = s0.r+s0.g+s0.b; if (sigma2 < min_sigma2) { min_sigma2 = sigma2; fragColor = m0;  }
     m1 /= n; s1 = abs(s1/n - m1*m1); sigma2 = s1.r+s1.g+s1.b; if (sigma2 < min_sigma2) { min_sigma2 = sigma2; fragColor = m1;  }
     m2 /= n; s2 = abs(s2/n - m2*m2); sigma2 = s2.r+s2.g+s2.b; if (sigma2 < min_sigma2) { min_sigma2 = sigma2; fragColor = m2;  }
     m3 /= n; s3 = abs(s3/n - m3*m3); sigma2 = s3.r+s3.g+s3.b; if (sigma2 < min_sigma2) { min_sigma2 = sigma2; fragColor = m3;  }
	 
	 
	 float3 hsl = rgb_to_hsl( fragColor.rgb );
	 hsl.g += g_s;  //Saturation
	 float3 ok = hsl_to_rgb(hsl);
		
	 return float4(ok,1);
}


technique tech_watercolor
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_watercolor();          
    }
}


//glsl mod != hlsl fmod
float hlsl_mod(float x, float y){  	return x - y * floor(x/y); }

/////////////////////////////////////////////////////////////////////////////////////

float g_kaleidoscope=1;
float g_kaleidTime=1;

float4 ps_kaleidoscope(VS_OUTPUT i , float2 vPos : VPOS ) : COLOR0
{
	const float TAU = 2.0 * PI;
	const float sections = max(1,g_kaleidoscope);

	 vec2 pos = i.Tex-float2(0.5,0.5);

	  float rad = length(pos);
	  float angle = atan2(pos.x,pos.y);

	  float ma = hlsl_mod(angle, TAU/sections);
	  ma = abs(ma - PI/sections);
	  
	  float x = cos(ma) * rad;
	  float y = sin(ma) * rad;
		
	  float time = g_time*g_kaleidTime;
	  
	  return  tex2D(g_samVideo, vec2(x-time, y-time));
}


technique tech_kaleidoscope
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_kaleidoscope();          
    }
}


////////////////////////////////////////////////////////////////////////////////////
float angle2radius(float angle )
{
	return  angle *(PI/180.0f);
}

float  g_circle_radius=0.1f;//[0.02,0.5]
float  g_circle_angle=5.0f; //[-300,300]
float  g_circle_angleEven=5.0f; //偶數圈[-300,300]
float  g_timeEven=0.0f;
float4 ps_circlecenter(VS_OUTPUT i , float2 vPos : VPOS ) : COLOR0
{
	float2 ok=i.Tex;
	float2 center  = float2(0.5f, 0.5f);
	float2 dif = i.Tex - center;
	float len = length(dif);
	
	int section  = len/g_circle_radius;
	
	if(section%2==0)//偶數圈
	{
		if(g_timeEven==0.0f)
		{ 
				ok = rotate2d( angle2radius(g_circle_angleEven), dif) + center;
		}
		else 	ok = rotate2d( angle2radius(g_circle_angleEven*g_timeEven), dif) + center;
	}
	else
	{
		if(g_time==0.0f)
		{ 
				ok = rotate2d( angle2radius(g_circle_angle), dif) + center;
		}
		else 	ok = rotate2d( angle2radius(g_circle_angle*g_time), dif) + center;
	}
	return tex2D(g_samVideo, ok );
}


technique tech_circlecenter
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_circlecenter();          
    }
}

////////////////////////////////////////////////////////////////////////////////////
float g_cyl_x=1, g_cyl_y=1;
float g_cy_dir=0;
VS_OUTPUT vs_cylinder( const VS_INPUT i )
{
    VS_OUTPUT o = (VS_OUTPUT)0;
	
	float4 f = mul( float4(i.iPos.xyz, 1), g_mObjWorld );//這不能互換
	o.Color = f;
    o.Pos = mul( f, g_mCameraViewProj );
    o.Tex = i.iTex;
	
    return o;
}

float4 ps_cylinder(VS_OUTPUT i , float2 vPos : VPOS ) : COLOR0
{
	float d = length( i.Color.xyz-g_eyepos.xyz );
	float color = lerp(1.0, 0.0, d/8.0f);
	
	float2 tex =  i.Tex;
	tex.y*=0.4;
	tex.x = tex.x+g_time*g_cyl_x;
	tex.y = tex.y+g_time*g_cyl_y;
	
	tex*=g_multiTex;


	return tex2D(g_samMirrorVideo, tex) *color;
}


technique tech_cylinder
{
    pass P0
    {
		//FillMode = WIREFRAME;
        CullMode = None;
        VertexShader = compile vs_3_0 vs_cylinder();
        PixelShader  = compile ps_3_0 ps_cylinder();          
    }
}

 


////////////////////////////////////////////////////////////////////////////////////

float g_plane_x=0;
float g_plane_y=0;

float4 ps_planerotate(VS_OUTPUT i , float2 vPos : VPOS ) : COLOR0
{
	float d = length( i.Color.xyz-g_eyepos.xyz );
	float color = lerp(1.3, 0, d/9.0f);

	
	float2 tex =  i.Tex;
	tex.x = tex.x+g_time*g_plane_x;
	tex.y = tex.y+g_time*g_plane_y;
	tex*=g_multiTex;

	return tex2D(g_samMirrorVideo, tex)*color;
}


technique tech_planerotate
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_cylinder();
        PixelShader  = compile ps_3_0 ps_planerotate();          
    }
}


////////////////////////////////////////////////////////////////////////////////////
 
int g_dotmosaics = 10;
float g_dotmosaics_bright=1.0f;
int  g_dotmosaics_kind=1;
float4 ps_dotmosaics(VS_OUTPUT i , float2 vPos : VPOS ) : COLOR0
{
	if( g_dotmosaics==0)return tex2D(g_samVideo, i.Tex);
	
	float r = (float)g_dotmosaics;
	float r2 = r*2;

	float cx=int(vPos.x/(int)r2)*r2+r;
	float cy=int(vPos.y/(int)r2)*r2+r;
	float2 center=float2(cx, cy);
	
	float4 ccc = tex2D(g_samVideo, float2(cx/g_video_w, cy/g_video_h) )*g_dotmosaics_bright;

	float d = distance( vPos, center);
	//sharp
	if(d<r)
	{
		float a= d/r;
		
		if( g_dotmosaics_kind==0  )return (1-a)*(1-a)*ccc;
		if( g_dotmosaics_kind==1  )return (1-a)*ccc;
		if( g_dotmosaics_kind==2  )return (1-a*a)  *ccc;
		if( g_dotmosaics_kind==3  )return (1-a*a*a )  *ccc;
		if( g_dotmosaics_kind==4  )return ccc;
	}
	
	
	return 0;
}


technique tech_dotmosaics
{
    pass P0
    {
		//FillMode = WIREFRAME;
        CullMode = None;
        VertexShader = compile vs_3_0 vs_cylinder();
        PixelShader  = compile ps_3_0 ps_dotmosaics();          
    }
}

///////////////////////////////////////////////////////////////////////////
float  g_singlediff=0.1;
float4  g_singleColor=float4(1,1,1,1);

float4 ps_singlecolor(VS_OUTPUT i) : COLOR0
{
	float3 color = tex2D(g_samVideo, i.Tex).rgb;

	
	float3 single=g_singleColor.rgb;
	 
	float gray = fun_gray(color);
	float3 g3 = float3(gray,gray,gray);
	
	float d = distance( color, single );
	
	const float mindiff =0.1f;
	if(  d <g_singlediff+mindiff )
	{
		if(d<g_singlediff){}
		else
		{
			 color = lerp( g3, color, 1.0f-(d-g_singlediff)/mindiff );
		}
		
		float3 hsl = rgb_to_hsl( color );
		hsl.g += g_s;  //Saturation
		float3 ok = hsl_to_rgb(hsl);
		return float4(ok, 1);	
	}
	 

	
	return float4(g3,1);
}
 
 
  
technique tech_singlecolor
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_singlecolor();          
    }
}

 
 
///////////////////////////////////////////////////////////////////////////
int g_nBlur=1;

float4 ps_blur(VS_OUTPUT i) : COLOR0
{
	float3 c[16];
	for(int j=0;j<16;j++)c[j]=float3(0,0,0);

	float3 color = tex2D(g_samVideo, i.Tex).rgb;	if( g_nBlur==0)return float4(color,1);
	
	c[0] = tex2D(g_samBlur0, i.Tex).rgb;
	c[1] = tex2D(g_samBlur1, i.Tex).rgb;
	c[2] = tex2D(g_samBlur2, i.Tex).rgb;
	c[3] = tex2D(g_samBlur3, i.Tex).rgb;
	c[4] = tex2D(g_samBlur4, i.Tex).rgb;
	c[5] = tex2D(g_samBlur5, i.Tex).rgb;
	c[6] = tex2D(g_samBlur6, i.Tex).rgb;
	c[7] = tex2D(g_samBlur7, i.Tex).rgb;
	c[8] = tex2D(g_samBlur8, i.Tex).rgb;
	c[9] = tex2D(g_samBlur9, i.Tex).rgb;
	
	if( g_nBlur>=10)
	{
		c[10] = tex2D(g_samBlur10, i.Tex).rgb;
		c[11] = tex2D(g_samBlur11, i.Tex).rgb;
		c[12] = tex2D(g_samBlur12, i.Tex).rgb;
		c[13] = tex2D(g_samBlur13, i.Tex).rgb;
		c[14] = tex2D(g_samBlur14, i.Tex).rgb;
		c[15] = color;
		
	}
	 
	
	
	float3 ok=float3(0,0,0);
	for(int j=0;j<g_nBlur;j++)ok +=c[j];

	ok/=g_nBlur;

	return float4(ok,1);

}
 
 
  
technique tech_blur
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_blur();          
    }
}

 
 
///////////////////////////////////////////////////////////////////////////
float g_mirage_angle=0.0f;
float g_mirage_range=0.8f;//0.4~1.55
float g_mirage_height=1.0f;
float g_mirage_xOffset=0.5f;
 

vec2 mirage_pixel()
{    
	float	u_dpi=1.0f;
	return float2(u_dpi, u_dpi)/float2(g_video_w, g_video_h);  
}  
	
float mirage_wave(float x,float freq, float speed)
{    
	return sin( x*freq + (g_time*10*(3.1415/2.0))*speed );  
}  
	
vec2 mirage_waves(float2 tex, float2 secTex )
{     
	secTex = rotate2d( angle2radius(g_mirage_angle),  secTex-float2(0.5, 0.5)) +float2(0.5, 0.5);
	float d = tex2D(g_samSecond, secTex).r;
	 
	
	float2 intensity=float2(g_mirage_height,1.0)*mirage_pixel()*d;
	
	float2 waves=float2
	(  	mirage_wave(tex.y,190.0, 0.35),   
		mirage_wave(tex.x,100.0, 0.4)    
	);    
	
	return tex+(waves*intensity);  
}  
 
	
float4 ps_mirage(VS_OUTPUT i, float2 vPos : VPOS ) : COLOR0
{
	 float2 secTex = i.Tex;
	 secTex.y += g_mirage_xOffset;
 	
	secTex = rotate2d( angle2radius(g_mirage_angle),  secTex-float2(0.5, 0.5))+float2(0.5, 0.5);
	
 
	secTex.y = 0.5+(secTex.y-0.5f) *g_mirage_range; 
	
	secTex=saturate(secTex);
	
	 //debug
	//float d = tex2D(g_samSecond, secTex).r;
	//return float4(d,d,d,1);
	

	vec2 turbulence=mirage_waves( i.Tex, secTex );    
	 vec4 c=tex2D(g_samVideo,turbulence);      
	 return vec4(c.rgb,1.0); 
}
 
 
  
technique tech_mirage
{
    pass P0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 vs_common();
        PixelShader  = compile ps_3_0 ps_mirage();          
    }
}


