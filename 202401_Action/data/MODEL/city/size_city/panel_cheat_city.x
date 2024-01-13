xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 52;
 -465.79151;38.14764;350.18108;,
 -465.79117;38.14764;-350.18077;,
 -465.79092;1.39497;-350.18091;,
 -465.79127;1.39497;350.18094;,
 -350.18101;38.14764;-465.79190;,
 -350.18067;1.39497;-465.79200;,
 350.18084;38.14764;-465.79132;,
 350.18113;1.39497;-465.79146;,
 465.79185;38.14764;-350.18096;,
 465.79210;1.39497;-350.18113;,
 465.79151;38.14764;350.18094;,
 465.79175;1.39497;350.18079;,
 350.18130;38.14764;465.79180;,
 350.18165;1.39497;465.79171;,
 -350.18079;38.14764;465.79136;,
 -350.18043;1.39497;465.79127;,
 -465.79151;38.14764;350.18108;,
 -465.79127;1.39497;350.18094;,
 0.00021;38.14764;-0.00003;,
 0.00021;38.14764;-0.00003;,
 0.00021;38.14764;-0.00003;,
 0.00021;38.14764;-0.00003;,
 0.00021;38.14764;-0.00003;,
 0.00021;38.14764;-0.00003;,
 0.00021;38.14764;-0.00003;,
 0.00021;38.14764;-0.00003;,
 0.00039;2.86064;-0.00011;,
 0.00039;2.86064;-0.00011;,
 0.00039;2.86064;-0.00011;,
 0.00039;2.86064;-0.00011;,
 0.00039;2.86064;-0.00011;,
 0.00039;2.86064;-0.00011;,
 0.00039;2.86064;-0.00011;,
 0.00039;2.86064;-0.00011;,
 -401.06004;58.04760;301.51619;,
 -401.05982;58.04760;-301.51595;,
 -401.05963;38.14764;-301.51607;,
 -401.05989;38.14764;301.51609;,
 -301.51609;58.04760;-401.06043;,
 -301.51582;38.14764;-401.06053;,
 301.51604;58.04760;-401.06002;,
 301.51629;38.14764;-401.06016;,
 401.06048;58.04760;-301.51612;,
 401.06063;38.14764;-301.51629;,
 401.06021;58.04760;301.51609;,
 401.06041;38.14764;301.51595;,
 301.51646;58.04760;401.06033;,
 301.51673;38.14764;401.06024;,
 -301.51592;58.04760;401.06002;,
 -301.51561;38.14764;401.05997;,
 0.00022;57.35400;-0.00004;,
 0.00037;38.14764;-0.00011;;
 
 48;
 4;0,1,2,3;,
 4;1,4,5,2;,
 4;4,6,7,5;,
 4;6,8,9,7;,
 4;8,10,11,9;,
 4;10,12,13,11;,
 4;12,14,15,13;,
 4;14,16,17,15;,
 3;18,1,0;,
 3;19,4,1;,
 3;20,6,4;,
 3;21,8,6;,
 3;22,10,8;,
 3;23,12,10;,
 3;24,14,12;,
 3;25,16,14;,
 3;26,3,2;,
 3;27,2,5;,
 3;28,5,7;,
 3;29,7,9;,
 3;30,9,11;,
 3;31,11,13;,
 3;32,13,15;,
 3;33,15,17;,
 4;34,35,36,37;,
 4;35,38,39,36;,
 4;38,40,41,39;,
 4;40,42,43,41;,
 4;42,44,45,43;,
 4;44,46,47,45;,
 4;46,48,49,47;,
 4;48,34,37,49;,
 3;50,35,34;,
 3;50,38,35;,
 3;50,40,38;,
 3;50,42,40;,
 3;50,44,42;,
 3;50,46,44;,
 3;50,48,46;,
 3;50,34,48;,
 3;51,37,36;,
 3;51,36,39;,
 3;51,39,41;,
 3;51,41,43;,
 3;51,43,45;,
 3;51,45,47;,
 3;51,47,49;,
 3;51,49,37;;
 
 MeshMaterialList {
  2;
  48;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1;;
  Material {
   0.412650;0.412650;0.412650;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.630000;0.630000;0.630000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\panel_logo.jpg";
   }
  }
 }
 MeshNormals {
  36;
  0.000000;1.000000;0.000000;,
  -0.923879;-0.000008;0.382684;,
  -0.923880;-0.000005;-0.382682;,
  -0.382685;0.000000;-0.923879;,
  0.382683;0.000006;-0.923880;,
  0.923879;0.000008;-0.382685;,
  0.923880;0.000005;0.382682;,
  0.382685;0.000000;0.923879;,
  -0.382683;-0.000006;0.923880;,
  0.000000;-1.000000;0.000000;,
  0.002471;-0.999997;-0.000898;,
  0.002471;-0.999997;0.000898;,
  0.000898;-0.999997;0.002471;,
  -0.000898;-0.999997;0.002471;,
  -0.002471;-0.999997;0.000898;,
  -0.002471;-0.999997;-0.000898;,
  -0.000898;-0.999997;-0.002471;,
  0.000898;-0.999997;-0.002471;,
  0.000000;1.000000;-0.000000;,
  -0.923879;-0.000011;0.382684;,
  -0.923880;-0.000007;-0.382682;,
  -0.382685;0.000001;-0.923879;,
  0.382683;0.000010;-0.923880;,
  0.923879;0.000011;-0.382684;,
  0.923880;0.000007;0.382682;,
  0.382684;0.000000;0.923879;,
  -0.382683;-0.000008;0.923880;,
  0.000000;-1.000000;-0.000000;,
  0.001358;0.999999;0.000494;,
  0.001358;0.999999;-0.000494;,
  0.000494;0.999999;0.001358;,
  -0.000494;0.999999;0.001358;,
  -0.001358;0.999999;0.000494;,
  -0.001358;0.999999;-0.000494;,
  -0.000494;0.999999;-0.001358;,
  0.000494;0.999999;-0.001358;;
  48;
  4;1,2,2,1;,
  4;2,3,3,2;,
  4;3,4,4,3;,
  4;4,5,5,4;,
  4;5,6,6,5;,
  4;6,7,7,6;,
  4;7,8,8,7;,
  4;8,1,1,8;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;9,10,11;,
  3;9,11,12;,
  3;9,12,13;,
  3;9,13,14;,
  3;9,14,15;,
  3;9,15,16;,
  3;9,16,17;,
  3;9,17,10;,
  4;19,20,20,19;,
  4;20,21,21,20;,
  4;21,22,22,21;,
  4;22,23,23,22;,
  4;23,24,24,23;,
  4;24,25,25,24;,
  4;25,26,26,25;,
  4;26,19,19,26;,
  3;18,28,29;,
  3;18,30,28;,
  3;18,31,30;,
  3;18,32,31;,
  3;18,33,32;,
  3;18,34,33;,
  3;18,35,34;,
  3;18,29,35;,
  3;27,27,27;,
  3;27,27,27;,
  3;27,27,27;,
  3;27,27,27;,
  3;27,27,27;,
  3;27,27,27;,
  3;27,27,27;,
  3;27,27,27;;
 }
 MeshTextureCoords {
  52;
  0.000000;0.000000;,
  0.125000;0.000000;,
  0.125000;1.000000;,
  0.000000;1.000000;,
  0.250000;0.000000;,
  0.250000;1.000000;,
  0.375000;0.000000;,
  0.375000;1.000000;,
  0.500000;0.000000;,
  0.500000;1.000000;,
  0.625000;0.000000;,
  0.625000;1.000000;,
  0.750000;0.000000;,
  0.750000;1.000000;,
  0.875000;0.000000;,
  0.875000;1.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.062500;0.000000;,
  0.187500;0.000000;,
  0.312500;0.000000;,
  0.437500;0.000000;,
  0.562500;0.000000;,
  0.687500;0.000000;,
  0.812500;0.000000;,
  0.937500;0.000000;,
  0.062500;1.000000;,
  0.187500;1.000000;,
  0.312500;1.000000;,
  0.437500;1.000000;,
  0.562500;1.000000;,
  0.687500;1.000000;,
  0.812500;1.000000;,
  0.937500;1.000000;,
  -0.151685;0.010064;,
  -0.151685;0.989935;,
  -0.151684;0.989935;,
  -0.151685;0.010065;,
  0.010065;1.151686;,
  0.010065;1.151686;,
  0.989935;1.151685;,
  0.989936;1.151685;,
  1.151686;0.989936;,
  1.151686;0.989936;,
  1.151685;0.010065;,
  1.151686;0.010065;,
  0.989936;-0.151686;,
  0.989936;-0.151685;,
  0.010065;-0.151685;,
  0.010065;-0.151685;,
  0.500000;0.500000;,
  0.500001;0.500000;;
 }
}
