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
 61;
 0.00000;203.17587;-60.73270;,
 110.23233;203.17587;-60.73270;,
 97.11190;92.94359;-60.73270;,
 0.00000;92.94359;-60.73270;,
 0.00000;203.17587;60.73270;,
 110.23233;203.17587;60.73270;,
 110.23233;203.17587;0.00000;,
 0.00000;203.17587;0.00000;,
 0.00000;92.94359;60.73270;,
 97.11190;92.94359;60.73270;,
 53.06071;0.00000;0.00000;,
 53.06071;0.00000;52.08904;,
 0.00000;0.00000;52.08904;,
 0.00000;0.00000;0.00000;,
 110.23233;203.17587;0.00000;,
 110.23233;203.17587;60.73270;,
 97.11190;92.94359;60.73270;,
 97.11190;92.94359;0.00000;,
 -97.11190;92.94359;60.73270;,
 -110.23233;203.17587;60.73270;,
 -110.23233;203.17587;0.00000;,
 -97.11190;92.94359;0.00000;,
 53.06071;0.00000;-52.08904;,
 0.00000;0.00000;-52.08904;,
 53.06071;0.00000;52.08904;,
 53.06071;0.00000;0.00000;,
 -53.06071;0.00000;52.08904;,
 -53.06071;0.00000;0.00000;,
 -53.06071;0.00000;52.08904;,
 -53.06071;0.00000;0.00000;,
 -97.11190;92.94359;60.73270;,
 -110.23233;203.17587;60.73270;,
 -110.23233;203.17587;0.00000;,
 -97.11190;92.94359;-60.73270;,
 -110.23233;203.17587;-60.73270;,
 -53.06071;0.00000;-52.08904;,
 0.00000;0.00000;-52.08904;,
 53.06071;0.00000;-52.08904;,
 -53.06071;0.00000;-52.08904;,
 -41.07161;195.83415;-41.07161;,
 -41.07161;210.55943;-41.07161;,
 41.07161;210.55943;-41.07161;,
 41.07161;195.83415;-41.07161;,
 -23.39518;241.92952;-23.39518;,
 -23.39518;241.92952;23.39518;,
 23.39518;241.92952;23.39518;,
 23.39518;241.92952;-23.39518;,
 -41.07161;210.55943;41.07161;,
 -41.07161;195.83415;41.07161;,
 41.07161;195.83415;41.07161;,
 41.07161;210.55943;41.07161;,
 -41.07161;195.83415;-41.07161;,
 41.07161;195.83415;-41.07161;,
 41.07161;210.55943;41.07161;,
 41.07161;195.83415;41.07161;,
 -41.07161;195.83415;41.07161;,
 -41.07161;210.55943;41.07161;,
 -29.01828;225.80425;-29.01828;,
 29.01828;225.80425;-29.01828;,
 29.01828;225.80425;29.01828;,
 -29.01828;225.80425;29.01828;;
 
 38;
 4;0,1,2,3;,
 4;4,5,6,7;,
 4;8,9,5,4;,
 4;10,11,12,13;,
 4;14,15,16,17;,
 4;18,19,20,21;,
 4;3,2,22,23;,
 4;17,16,24,25;,
 4;12,11,9,8;,
 4;26,18,21,27;,
 4;28,29,13,12;,
 4;30,28,12,8;,
 4;31,30,8,4;,
 4;32,31,4,7;,
 4;33,34,0,3;,
 4;35,33,3,23;,
 4;34,32,7,0;,
 4;6,1,0,7;,
 4;2,1,14,17;,
 4;22,2,17,25;,
 4;36,37,10,13;,
 4;29,38,36,13;,
 4;21,33,35,27;,
 4;20,34,33,21;,
 4;39,40,41,42;,
 4;43,44,45,46;,
 4;47,48,49,50;,
 4;48,51,52,49;,
 4;42,41,53,54;,
 4;55,56,40,39;,
 4;40,57,58,41;,
 4;41,58,59,50;,
 4;50,59,60,47;,
 4;47,60,57,40;,
 4;57,43,46,58;,
 4;58,46,45,59;,
 4;59,45,44,60;,
 4;60,44,43,57;;
 
 MeshMaterialList {
  1;
  38;
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
  0;;
  Material {
   0.400000;0.400000;0.400000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  30;
  0.000000;-0.092599;-0.995703;,
  0.000000;0.000000;-1.000000;,
  -0.992991;-0.118191;0.000000;,
  0.000000;1.000000;0.000000;,
  -0.903643;-0.428287;0.000000;,
  0.000000;-1.000000;-0.000000;,
  0.000000;-0.046349;-0.998925;,
  0.000000;-0.046349;0.998925;,
  -0.960908;-0.276867;0.000000;,
  -0.992991;-0.118191;0.000000;,
  0.960908;-0.276867;-0.000000;,
  0.000000;0.328304;-0.944572;,
  0.000000;0.328304;0.944572;,
  0.000000;-0.000000;1.000000;,
  0.000000;0.481417;-0.876491;,
  0.876491;0.481417;0.000000;,
  0.000000;0.481417;0.876491;,
  0.992991;-0.118191;-0.000000;,
  0.992991;-0.118191;-0.000000;,
  0.903643;-0.428287;-0.000000;,
  0.000000;-0.092599;0.995703;,
  1.000000;0.000000;0.000000;,
  0.944572;0.328304;0.000000;,
  -1.000000;0.000000;0.000000;,
  -0.944572;0.328304;0.000000;,
  -0.876491;0.481417;0.000000;,
  0.000000;0.329268;-0.944236;,
  0.944236;0.329268;0.000000;,
  0.000000;0.329268;0.944236;,
  -0.944236;0.329268;0.000000;;
  38;
  4;1,1,6,6;,
  4;3,3,3,3;,
  4;7,7,13,13;,
  4;5,5,5,5;,
  4;17,18,10,10;,
  4;8,2,9,8;,
  4;6,6,0,0;,
  4;10,10,19,19;,
  4;20,20,7,7;,
  4;4,8,8,4;,
  4;5,5,5,5;,
  4;7,20,20,7;,
  4;13,7,7,13;,
  4;3,3,3,3;,
  4;6,1,1,6;,
  4;0,6,6,0;,
  4;3,3,3,3;,
  4;3,3,3,3;,
  4;10,18,17,10;,
  4;19,10,10,19;,
  4;5,5,5,5;,
  4;5,5,5,5;,
  4;8,8,4,4;,
  4;9,2,8,8;,
  4;1,11,11,1;,
  4;3,3,3,3;,
  4;12,13,13,12;,
  4;5,5,5,5;,
  4;21,22,22,21;,
  4;23,24,24,23;,
  4;11,14,14,11;,
  4;22,15,15,22;,
  4;12,16,16,12;,
  4;24,25,25,24;,
  4;14,26,26,14;,
  4;15,27,27,15;,
  4;16,28,28,16;,
  4;25,29,29,25;;
 }
 MeshTextureCoords {
  61;
  0.500000;0.750000;,
  0.625000;0.750000;,
  0.625000;0.875000;,
  0.500000;0.875000;,
  0.500000;0.500000;,
  0.625000;0.500000;,
  0.625000;0.625000;,
  0.500000;0.625000;,
  0.500000;0.375000;,
  0.625000;0.375000;,
  0.625000;0.125000;,
  0.625000;0.250000;,
  0.500000;0.250000;,
  0.500000;0.125000;,
  0.750000;0.750000;,
  0.875000;0.750000;,
  0.875000;0.875000;,
  0.750000;0.875000;,
  0.125000;0.875000;,
  0.125000;0.750000;,
  0.250000;0.750000;,
  0.250000;0.875000;,
  0.625000;1.000000;,
  0.500000;1.000000;,
  0.875000;1.000000;,
  0.750000;1.000000;,
  0.125000;1.000000;,
  0.250000;1.000000;,
  0.375000;0.250000;,
  0.375000;0.125000;,
  0.375000;0.375000;,
  0.375000;0.500000;,
  0.375000;0.625000;,
  0.375000;0.875000;,
  0.375000;0.750000;,
  0.375000;1.000000;,
  0.500000;0.000000;,
  0.625000;0.000000;,
  0.375000;0.000000;,
  0.375000;1.000000;,
  0.375000;0.750000;,
  0.625000;0.750000;,
  0.625000;1.000000;,
  0.375000;0.750000;,
  0.375000;0.500000;,
  0.625000;0.500000;,
  0.625000;0.750000;,
  0.375000;0.500000;,
  0.375000;0.250000;,
  0.625000;0.250000;,
  0.625000;0.500000;,
  0.375000;0.000000;,
  0.625000;0.000000;,
  0.875000;0.750000;,
  0.875000;1.000000;,
  0.125000;1.000000;,
  0.125000;0.750000;,
  0.375000;0.750000;,
  0.625000;0.750000;,
  0.625000;0.500000;,
  0.375000;0.500000;;
 }
}
