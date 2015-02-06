#include "StdAfx.h"
#include "XImagePacker.h"
#include <iostream>
#include <assert.h>
#include <io.h>
#include <algorithm>

XImagePacker gImagePacker;

XImagePacker::XImagePacker ( void )
    : mSpace ( 0 )
{
    mOutWidth = 1024;
    mOutHeight = 1024;
    // Set FreeImage message handler
    FreeImage_SetOutputMessage ( errorReport );
}

XImagePacker::~XImagePacker ( void )
{
    dSafeDeleteVector ( mTextureArray );
}

void XImagePacker::addTexture ( const char* filename, const char* parent /*= nullptr */ )
{
    IPTexture* texture = new IPTexture;
    CXFileName xfilename ( filename );

    texture->mFileName = xfilename.GetRelativeFileName();
    texture->loadData();
    if ( texture->getData() == nullptr )
    {
        delete texture;
        return;
    }
    else
    {
        mTextureArray.push_back ( texture );
    }
    onAdd ( xfilename.GetRelativeFileName(), parent );
}


bool compare_texture ( IPTexture* lhs, IPTexture* rhs )
{
    return lhs->mRect.area() > rhs->mRect.area();
}

void XImagePacker::sortTextures()
{
    std::sort ( mTextureArray.begin(), mTextureArray.end(), compare_texture );
}

bool XImagePacker::saveAll()
{
    FILE* output_text = 0;

    printf ( "Creating output texture for atlas %s\n", mOutTextureFile.c_str() );

    FIBITMAP* output_texture = FreeImage_Allocate ( mOutWidth, mOutHeight, 32, 0, 0, 0 );

    if ( fopen_s ( &output_text, mOutConfigFile.c_str(), "w" ) )
    {
        assert ( 0 && "can not open file!" );
        return false;
    }

    size_t count = mTextureArray.size();
    for ( size_t i = 0; i < count; ++i )
    {
        IPTexture* tex = mTextureArray[i];
        fprintf ( output_text, "\"%s\" %d %d %d\n", tex->mFileName.c_str(), tex->mRect.mX, tex->mRect.mY, tex->getPixelSize() );
        FreeImage_Paste ( output_texture, tex->getData(), tex->mRect.mX, tex->mRect.mY, 255 );
    }

    printf ( "Writing texture atlas to %s\n", mOutTextureFile.c_str() );

    if ( !FreeImage_Save ( mOutFormat, output_texture, mOutTextureFile.c_str(), 0 ) )
    {
        fprintf ( stderr, "Unable to save image %s\n", mOutTextureFile.c_str() );

        return false;
    }

    FreeImage_Unload ( output_texture );
    fclose ( output_text );

    return true;
}



void XImagePacker::errorReport ( FREE_IMAGE_FORMAT fif, const char* message )
{

}

void XImagePacker::save ( const char* outTextureName, const char* outTextFile, int outW, int outH, FREE_IMAGE_FORMAT fmt )
{
    mOutTextureFile = outTextureName;
    mOutConfigFile = outTextFile;
    mOutWidth = outW;
    mOutHeight = outH;
    mOutFormat = fmt;
    /** 重新计算一遍 **/
    onAdd ( nullptr );
    saveAll();
}


bool XImagePacker::adpotWithRes ( IPTexture* curTexture, std::vector<IPTexture*>& res )
{
    bool ok = false;
    size_t n = 0;
    //放下面
    for ( ; n < res.size(); ++n )
    {
        IPTexture* txt = res[n];

        if ( curTexture == txt )
            continue;

        curTexture->mRect.mX = txt->mRect.mX;
        curTexture->mRect.mY = txt->mRect.bottom() + mSpace;

        bool overlap = false;

        for ( size_t i = 0; i < res.size(); ++i )
        {
            IPTexture* pt = res[i];
            if ( pt == curTexture )
                continue;
            if ( curTexture->mRect.isOverlaps ( pt->mRect ) || !isInCanvos ( curTexture->mRect ) )
            {
                overlap = true;
            }
        }

        if ( overlap )
        {
            continue;
        }
        else
        {
            ok = true;
            break;
        }
    }
    //放右面
    if ( !ok )
    {
        n = 0;
        for ( ; n < res.size(); ++n )
        {
            IPTexture* txt = res[n];

            if ( curTexture == txt )
                continue;

            curTexture->mRect.mY = txt->mRect.mY;
            curTexture->mRect.mX = txt->mRect.right() + mSpace;

            bool overlap = false;

            for ( size_t i = 0; i < res.size(); ++i )
            {
                IPTexture* pt = res[i];
                if ( pt == curTexture )
                    continue;
                if ( curTexture->mRect.isOverlaps ( pt->mRect ) || !isInCanvos ( curTexture->mRect ) )
                {
                    overlap = true;
                }
            }

            if ( overlap )
            {
                continue;
            }
            else
            {
                ok = true;
                break;
            }
        }
    }

    if ( ok )
    {
        res.push_back ( curTexture );
    }
    return ok;
}
bool XImagePacker::calcPos()
{
    size_t cnt = mTextureArray.size();
    for ( size_t i = 0; i < cnt; ++i )
    {
        IPTexture* curTexture = mTextureArray[i];
        curTexture->mRect.mX = 0;
        curTexture->mRect.mY = 0;
        if ( !isInCanvos ( curTexture->mRect ) )
        {
            //assert ( 0 && "Canvos is to small!" );
            return false;
        }
    }

    std::vector<IPTexture*> res;

    for ( size_t i = 0; i < cnt; ++i )
    {
        IPTexture* curTexture = mTextureArray[i];
        if ( !isInCanvos ( curTexture->mRect ) )
        {
            //assert ( 0 && "Canvos is to small!" );
            return false;
        }
        else
        {
            if ( res.size() == 0 )
            {
                res.push_back ( curTexture );
                continue;
            }
        }
        if ( !adpotWithRes ( curTexture, res ) )
        {
            return false;
        }
    }
    return true;
}

const std::vector<IPTexture*>& XImagePacker::getAllTexture() const
{
    return mTextureArray;
}

bool XImagePacker::isInCanvos ( const CXRect& rc )
{
    return rc.mX >= 0 && rc.mY >= 0 && rc.right() <= mOutWidth && rc.bottom() <= mOutHeight;
}
bool isOrignalNameTrue ( XImageTree::Node* node, const char* name )
{
    return node->getData()->mOrignalName == name;
}
void XImagePacker::addPath ( const char* path, bool traverse /*= true*/, const char* parent /*= nullptr */ )
{
    CXAddTextureArg arg;
    arg.mName = path;
    if ( parent != nullptr )
        arg.mParent = parent;

    mDelegateAddPath.trigger ( &arg );

    CXFileName filepath ( path );

    IPTextureNode* node = new IPTextureNode;
    node->mDisplayName = filepath.GetFileName();
    node->mOrignalName = filepath.GetRelativeFileName();

    CXTreeNode<IPTextureNode>* treeNode = new CXTreeNode<IPTextureNode>();
    treeNode->setData ( node );

    if ( parent != nullptr )
    {
        CXTreeNode<IPTextureNode>* parentNode = nullptr;
        if ( mTree.findChild ( parentNode, isOrignalNameTrue, parent ) )
        {
            parentNode->addChild ( treeNode );
        }
    }
    else
    {
        mTree.mNodes.push_back ( treeNode );
    }

    if ( !traverse )
    {
        return;
    }

    _finddata_t allFile;
    intptr_t hFile;
    std::string target = path;
    target += "/*.*";
    std::string filename;
    if ( ( hFile = _findfirst ( target.c_str(), &allFile ) ) == -1L )
        printf ( "No *.c files in current directory!\n" );
    else
    {
        do
        {
            filename = path;
            filename += "/";
            filename += allFile.name;
            if ( !dStrEqual ( allFile.name, "." ) && !dStrEqual ( allFile.name, ".." ) )
            {
                CXFileName pather ( filename.c_str() );
                GString parentPath;
                if ( dIsPath ( filename.c_str() ) )
                {
                    if ( pather.GetParentPath ( parentPath ) )
                    {
                        addPath ( filename.c_str(), traverse, pather.GetRelativePath() );
                    }
                    else
                    {
                        addPath ( filename.c_str(), traverse );
                    }
                }
                else
                {
                    if ( pather.GetParentPath ( parentPath ) )
                    {
                        addTexture ( filename.c_str(), pather.GetRelativePath() );
                    }
                    else
                    {
                        addTexture ( filename.c_str() );
                    }
                }
            }
        }
        while ( _findnext ( hFile, &allFile ) == 0 );
        _findclose ( hFile );
    }
    //onAdd();
}

IPTexture* XImagePacker::getTexture ( const char* name )
{
for ( auto tex: mTextureArray )
    {
        if ( tex->mFileName == name  )
        {
            return tex;
        }
    }
    return nullptr;
}

void XImagePacker::onAdd ( const char* texName, const char* parent /*= nullptr*/ )
{
    sortTextures();

    if ( texName != nullptr )
    {
        if ( calcPos() )
        {
            addTextureTreeNode ( texName, parent );

            CXAddTextureArg arg;
            arg.mName = texName;
            if ( parent != nullptr )
                arg.mParent = parent;
            mDelegateAddTexture.trigger ( &arg );
        }
        else
        {
            deleteTextureInner ( texName );
            mDelegateAddTextureFailed.trigger ( );
        }
    }
}

void XImagePacker::deleteTextureInner ( const char* name )
{
    for ( auto i = mTextureArray.begin(); i != mTextureArray.end(); ++i )
    {
        auto tex = *i;
        if ( tex->mFileName == name )
        {
            delete tex;
            mTextureArray.erase ( i );
            break;
        }
    }
}
#define _Project "Project"
#define _Floder "Floder"
#define _File	"File"
#define _Name "Name"

void XImagePacker::saveProjectFile ( const char* name )
{
    CXRapidxmlDocument doc;
    doc.append_node (
        doc.allocate_node ( rapidxml::node_pi, doc.allocate_string ( "xml version=\"1.0\" encoding=\"UTF-8\"" ) )
    );
    CXRapidxmlNode* root = doc.allocate_node ( rapidxml::node_element );
    root->name ( _Project );
    doc.append_node ( root );
for ( auto i: mTree.mNodes )
    {
        linkTo ( i, root );
    }
    CXRapidxmlWriter writer;
    writer.AppendChild ( &doc );
    writer.Write ( name );
}


void XImagePacker::loadProjectFile ( const char* name )
{
    //xml_load ( name );

    //xml_get_node ( _FileList )
    //{
    //    xml_get_node ( _Floder )
    //    {
    //        GString path;
    //        xml_get_attr ( _Name, path );
    //        addPath ( path.c_str() );


    //        xml_get_node ( _File )
    //        {
    //            xml_get_attr ( _Name, )
    //        }
    //    }
    //}
}

void XImagePacker::reset()
{
    mTextureArray.destroy();
    mTree.destroy();
}

void XImagePacker::linkTo ( XImageTree::Node* treeNode, CXRapidxmlNode* docNode )
{
    if ( treeNode->getData()->isPath() && treeNode->mChildren.empty() )
    {
        return;
    }

    CXRapidxmlNode* me = docNode->document()->allocate_node ( rapidxml::node_element );

    me->name ( treeNode->getData()->isPath() ? _Floder : _File );

    CXRapidxmlAttr* attrType = docNode->document()->allocate_attribute ( _Name, treeNode->getData()->mOrignalName );

    me->append_attribute ( attrType );

    docNode->append_node ( me );

for ( auto i: treeNode->mChildren )
    {
        linkTo ( i, me );
    }
}

void XImagePacker::addTextureTreeNode ( const char* texName, const char* parent /*= nullptr */ )
{
    CXFileName filepath ( texName );
    IPTexture* node = new IPTexture;
    node->mDisplayName = filepath.GetFileName();
    node->mOrignalName = filepath.GetRelativeFileName();
    CXTreeNode<IPTextureNode>* treeNode = new CXTreeNode<IPTextureNode>();
    treeNode->setData ( node );
    if ( parent != nullptr )
    {
        CXTreeNode<IPTextureNode>* parentNode = nullptr;
        if ( mTree.findChild ( parentNode, isOrignalNameTrue, parent ) )
        {
            parentNode->addChild ( treeNode );
        }
    }
    else
    {
        mTree.mNodes.push_back ( treeNode );
    }
}





IPTexture::~IPTexture()
{
    unloadData();
}

void IPTexture::unloadData()
{
    if ( mData != nullptr )
    {
        FreeImage_Unload ( mData );
        mData = nullptr;
    }
}

IPTexture::IPTexture()
{
    mData = nullptr;
}

bool IPTexture::loadData()
{
    FREE_IMAGE_FORMAT fmt;

    fmt = FreeImage_GetFIFFromFilename ( mFileName.c_str() );

    if ( fmt == FIF_UNKNOWN )
    {
        return false;
    }
    mData = FreeImage_Load ( fmt, mFileName.c_str(), 0 );

    setSize ( getWByData(), getHByData() );

    return true;
}



