/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2017, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

// Actually just a dummy, used by the compiler to build the precompiled header.

#include <assimp/version.h>
#include <assimp/scene.h>
#include "ScenePrivate.h"

// ------------------------------------------------------------------------------------------------
ASSIMP_API aiScene::aiScene()
: mFlags(0)
, mRootNode(NULL)
, mNumMeshes(0)
, mMeshes(NULL)
, mNumMaterials(0)
, mMaterials(NULL)
, mNumAnimations(0)
, mAnimations(NULL)
, mNumTextures(0)
, mTextures(NULL)
, mNumLights(0)
, mLights(NULL)
, mNumCameras(0)
, mCameras(NULL)
, mPrivate(new Assimp::ScenePrivateData()) {
	// empty
}

// ------------------------------------------------------------------------------------------------
ASSIMP_API aiScene::~aiScene() {
    // delete all sub-objects recursively
    delete mRootNode;

    // To make sure we won't crash if the data is invalid it's
    // much better to check whether both mNumXXX and mXXX are
    // valid instead of relying on just one of them.
    if (mNumMeshes && mMeshes)
        for( unsigned int a = 0; a < mNumMeshes; a++)
            delete mMeshes[a];
    delete [] mMeshes;

    if (mNumMaterials && mMaterials)
        for( unsigned int a = 0; a < mNumMaterials; a++)
            delete mMaterials[a];
    delete [] mMaterials;

    if (mNumAnimations && mAnimations)
        for( unsigned int a = 0; a < mNumAnimations; a++)
            delete mAnimations[a];
    delete [] mAnimations;

    if (mNumTextures && mTextures)
        for( unsigned int a = 0; a < mNumTextures; a++)
            delete mTextures[a];
    delete [] mTextures;

    if (mNumLights && mLights)
        for( unsigned int a = 0; a < mNumLights; a++)
            delete mLights[a];
    delete [] mLights;

    if (mNumCameras && mCameras)
        for( unsigned int a = 0; a < mNumCameras; a++)
            delete mCameras[a];
    delete [] mCameras;

    delete static_cast<Assimp::ScenePrivateData*>( mPrivate );
}

