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
    mOutFormat = FIF_TARGA;
    // Set FreeImage message handler
    FreeImage_SetOutputMessage ( errorReport );
}

XImagePacker::~XImagePacker ( void )
{
    mTextureArray.clear();
}
bool isOrignalNameTrue ( XImageTree::Node* node, const char* name )
{
    return dStrEqual ( node->getData()->getRawName(), name );
}


bool compare_texture ( IPTextureNode* lhs, IPTextureNode* rhs )
{
    return lhs->mRect.area() > rhs->mRect.area();
}

void XImagePacker::sortTextures()
{
    std::sort ( mTextureArray.begin(), mTextureArray.end(), compare_texture );
}

bool XImagePacker::saveImage()
{
    FIBITMAP* output_texture = FreeImage_Allocate ( mOutWidth, mOutHeight, 32, 0, 0, 0 );

    size_t count = mTextureArray.size();

    for ( size_t i = 0; i < count; ++i )
    {
        IPTexture* tex = ( IPTexture* ) mTextureArray[i];
        FreeImage_Paste ( output_texture, tex->getData(), tex->mRect.mX, tex->mRect.mY, 255 );
    }

    if ( !FreeImage_Save ( mOutFormat, output_texture, mImageFile.c_str(), 0 ) )
    {
        return false;
    }

    FreeImage_Unload ( output_texture );

    return true;
}



void XImagePacker::errorReport ( FREE_IMAGE_FORMAT fif, const char* message )
{

}

void XImagePacker::save ( const char* outTextureName, const char* projectFile )
{
    mImageFile = outTextureName;
    mProjectFile = projectFile;

    /** 重新计算一遍 **/
    tryToAddTexture ( nullptr );

    saveImage();
    saveProject();
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
        IPTexture* curTexture = ( IPTexture* ) mTextureArray[i];
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
        IPTexture* curTexture = ( IPTexture* ) mTextureArray[i];
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


bool XImagePacker::isInCanvos ( const CXRect& rc )
{
    return rc.mX >= 0 && rc.mY >= 0 && rc.right() <= mOutWidth && rc.bottom() <= mOutHeight;
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
    node->setDisplayName ( filepath.GetFileName() );
    node->setRawName ( filepath.GetRelativeFileName() );

    CXTreeNode<IPTextureNode>* treeNode = new CXTreeNode<IPTextureNode>();
    treeNode->setData ( node );

    if ( parent != nullptr )
    {
        CXTreeNode<IPTextureNode>* parentNode = getNodeInTree ( parent );
        if ( parentNode )
            parentNode->addChild ( treeNode );
    }
    else
    {
        mTree.add ( treeNode );
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
                        tryToAddTexture ( pather.GetRelativeFileName(), pather.GetRelativePath() );
                    }
                    else
                    {
                        tryToAddTexture ( pather.GetRelativeFileName() );
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
    XImageTree::Node* treeNode = getNodeInTree ( name );
    if ( treeNode )
    {
        if ( !treeNode->getData()->isPath() )
        {
            return ( IPTexture* ) treeNode->getData();
        }
    }
    return nullptr;
}

bool XImagePacker::tryToAddTexture ( const char* texName, const char* parent /*= nullptr */ )
{
    if ( texName != nullptr )
    {
        if ( tryToAddTextureTreeNode ( texName, parent ) )
        {
            refillData();

            sortTextures();

            if ( calcPos() )
            {
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
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

void XImagePacker::deleteTextureInner ( const char* name )
{
for ( auto i: mTextureArray )
    {
        if ( dStrEqual ( name, i->getRawName() ) )
        {
            mTextureArray.remove ( i );
            break;
        }
    }
    mTree.deleteChild ( isOrignalNameTrue, name );
}
#define _Project "Project"
#define _OutImage "OutImage"
#define _Floder "Floder"
#define _File	"File"
#define _Name "Name"
#define _X "x"
#define _Y "y"
#define _W "w"
#define _H "h"

void XImagePacker::saveProject()
{
    CXASSERT_RESULT ( !mImageFile.empty() );
    CXASSERT_RESULT ( !mProjectFile.empty() );

    CXRapidxmlDocument doc;
    doc.append_node (
        doc.allocate_node ( rapidxml::node_pi, doc.allocate_string ( "xml version=\"1.0\" encoding=\"UTF-8\"" ) )
    );
    CXRapidxmlNode* root = doc.allocate_node ( rapidxml::node_element );
    root->name ( _Project );
    doc.append_node ( root );
    CXFileName pathfile ( mImageFile.c_str() );

    CXRapidxmlAttr* attr = doc.allocate_attribute (
                               _OutImage
                               , doc.allocate_string ( pathfile.GetRelativeFileName() )
                           );
    root->append_attribute ( attr );
    auto nodes = mTree.getNodes();
for ( auto i: nodes )
    {
        linkTo ( i, root );
    }
    CXRapidxmlWriter writer;
    writer.AppendChild ( &doc );
    writer.Write ( mProjectFile.c_str() );
}

bool XImagePacker::loadNode ( CXRapidxmlNode* _node, const GString& parentPath )
{
    if ( _node == nullptr )
        return false;
    CXXMLNodeStack _nodeStack;
    const char* parentP = parentPath.isEmpty() ? nullptr : parentPath.c_str();
    xml_get_node ( _Floder )
    {
        GString path;
        xml_get_attr ( _Name, path );
        addPath ( path.c_str(), false, parentP );
        if ( !loadNode ( _node, path ) )
            return false;
        xml_get_node ( _File )
        {
            GString name;
            xml_get_attr ( _Name, name );
            tryToAddTexture ( name, path );
        }
    }
    xml_get_node ( _File )
    {
        GString name;
        xml_get_attr ( _Name, name );
        tryToAddTexture ( name, parentP );
    }
    return true;
}
bool XImagePacker::loadProject ( const char* name )
{
    this->reset();
    CXRapidxmlLoader loader ( name );
    CXASSERT ( loader.loadFile() );
    CXXMLNodeStack _nodeStack;
    CXRapidxmlNode* _node = loader.getRootNode();

    xml_get_attr ( _OutImage, mImageFile );

    if ( !loadNode ( _node ) )
        return false;

    //xml_get_node ( _Floder )
    //{
    //    GString path;
    //    xml_get_attr ( _Name, path );
    //    addPath ( path.c_str(), false );
    //    xml_get_node ( _File )
    //    {
    //        GString name;
    //        xml_get_attr ( _Name, name );
    //        tryToAddTexture ( name, path );
    //    }
    //}
    //xml_get_node ( _File )
    //{
    //    GString name;
    //    xml_get_attr ( _Name, name );
    //    tryToAddTexture ( name );
    //}
    mProjectFile = name;
    mDelegateLoadSuccessed.trigger();
    return true;
}

void XImagePacker::reset()
{
    mTree.destroy();
    mTextureArray.clear();
    mDelegateClear.trigger();
}

void XImagePacker::linkTo ( XImageTree::Node* treeNode, CXRapidxmlNode* docNode )
{
    if ( treeNode->getData()->isPath() && treeNode->childCount() == 0 )
    {
        return;
    }

    CXRapidxmlDocument* doc = docNode->document();

    CXRapidxmlNode* me = doc->allocate_node ( rapidxml::node_element );

    bool bpath = treeNode->getData()->isPath();

    me->name ( bpath ? _Floder : _File );

    CXRapidxmlAttr* attrType = doc->allocate_attribute ( _Name, treeNode->getData()->getRawName() );

    me->append_attribute ( attrType );

    GString satt;
    if ( !bpath )
    {
        IPTexture* tex = ( IPTexture* ) treeNode->getData();
        satt.Format ( "%d", tex->mRect.mX );
        CXRapidxmlAttr* attrX = doc->allocate_attribute ( _X,  doc->allocate_string ( satt.c_str() ) );
        satt.Format ( "%d", tex->mRect.mY );
        CXRapidxmlAttr* attrY = doc->allocate_attribute ( _Y, doc->allocate_string ( satt.c_str() ) );
        satt.Format ( "%d", tex->mRect.mW );
        CXRapidxmlAttr* attrW = doc->allocate_attribute ( _W, doc->allocate_string ( satt.c_str() ) );
        satt.Format ( "%d", tex->mRect.mH );
        CXRapidxmlAttr* attrH = doc->allocate_attribute ( _H, doc->allocate_string ( satt.c_str() ) );
        me->append_attribute ( attrX );
        me->append_attribute ( attrY );
        me->append_attribute ( attrW );
        me->append_attribute ( attrH );
    }

    docNode->append_node ( me );
    auto children = treeNode->children();
for ( auto i: children )
    {
        linkTo ( i, me );
    }
}

bool XImagePacker::tryToAddTextureTreeNode ( const char* texName, const char* parent /*= nullptr */ )
{
    if ( getNodeInTree ( texName ) )
        return false;

    CXFileName filepath ( texName );
    IPTexture* node = new IPTexture;
    node->setRawName ( filepath.GetRelativeFileName() );
    node->setDisplayName ( filepath.GetFileName() );
    node->loadData();

    if ( node->getData() == nullptr )
    {
        delete node;
        return false;
    }

    CXTreeNode<IPTextureNode>* treeNode = new CXTreeNode<IPTextureNode>();
    treeNode->setData ( node );
    if ( parent != nullptr )
    {
        CXTreeNode<IPTextureNode>* parentNode = getNodeInTree ( parent );
        if ( parentNode )
            parentNode->addChild ( treeNode );
    }
    else
    {
        mTree.add ( treeNode );
    }
    return true;
}

void XImagePacker::refillData()
{
    mTextureArray.clear();
    CXDynaArray<IPTextureNode*> rawArray;
    mTree.toArray ( rawArray );
for ( auto i: rawArray )
    {
        if ( !i->isPath() )
        {
            mTextureArray.push_back ( ( IPTexture* ) i );
        }
    }
}

void XImagePacker::setOutFormat ( int w, int h, FREE_IMAGE_FORMAT fmt )
{
    mOutWidth = w;
    mOutHeight = h;
    mOutFormat = fmt;
}

const char* XImagePacker::getImageFile()
{
    return mImageFile;
}

const char* XImagePacker::getProjectFile()
{
    return mProjectFile;
}

CXTreeNode<IPTextureNode>* XImagePacker::getNodeInTree ( const char* filename )
{
    CXFileName filepath ( filename );
    CXTreeNode<IPTextureNode>* node = nullptr;
    mTree.findChild ( node, isOrignalNameTrue, filepath.GetRelativeFileName() );
    return node;
}

void XImagePacker::deleteImage ( const char* name )
{
    CXTreeNode<IPTextureNode>* node = nullptr;
    if ( mTree.findChild ( node, isOrignalNameTrue, name ) )
    {
        deleteTextureInner ( name );
        refillData();
        sortTextures();
        CXASSERT_RETURN ( calcPos() );

        CXDeleteTextureArg arg;
        arg.mName = name;
        gImagePacker.mDelegateDeleteTexture.trigger ( &arg );
    }
}
bool isRectTrue ( XImageTree::Node* node, const CXRect& rc )
{
    return node->getData()->mRect == rc;
}
IPTextureNode* XImagePacker::getImageByRect ( const CXRect& rect )
{
    XImageTree::Node* child = nullptr;
    if ( mTree.findChild ( child, isRectTrue, rect ) )
    {
        return child->getData();
    }
    return nullptr;
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

    fmt = FreeImage_GetFIFFromFilename ( mOrignalName.c_str() );

    if ( fmt == FIF_UNKNOWN )
    {
        return false;
    }
    mData = FreeImage_Load ( fmt, mOrignalName.c_str(), 0 );

    setSize ( getWByData(), getHByData() );

    return true;
}



