#include "slang-implicit-directory-collector.h"

namespace Slang
{

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ImplicitDirectoryCollector !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

ImplicitDirectoryCollector::ImplicitDirectoryCollector(const String& canonicalPath, bool directoryExists) :
    m_directoryExists(directoryExists)
{
    StringBuilder buffer;
    if (canonicalPath != ".")
    {
        buffer << canonicalPath;
        buffer.append('/');
    }
    m_prefix = buffer.ProduceString();
}

void ImplicitDirectoryCollector::addRemainingPath(SlangPathType pathType, const UnownedStringSlice& inPathRemainder)
{
    // If it's zero length we probably don't want to add it
    if (inPathRemainder.getLength() == 0)
    {
        // It's empty so don't add normal way - implies the directory exists
        m_directoryExists = true;
        return;
    }

    UnownedStringSlice pathRemainder(inPathRemainder);
    const Index slashIndex = pathRemainder.indexOf('/');

    // If we have a following / that means it's an implicit directory.
    if (slashIndex >= 0)
    {
        pathType = SLANG_PATH_TYPE_DIRECTORY;
        pathRemainder = UnownedStringSlice(pathRemainder.begin(), pathRemainder.begin() + slashIndex);
    }

    const Index countIndex = m_map.findOrAdd(pathRemainder, pathType);
    SLANG_UNUSED(countIndex);
    // Make sure they are the same type
    SLANG_ASSERT(SlangPathType(m_map.getValueAt(countIndex)) == pathType);
}

void ImplicitDirectoryCollector::addPath(SlangPathType pathType, const UnownedStringSlice& canonicalPath)
{
    if (canonicalPath != toSlice(".") && hasPrefix(canonicalPath))
    {
        UnownedStringSlice remainder = getRemainder(canonicalPath);
        addRemainingPath(pathType, remainder);
    }
}

SlangResult ImplicitDirectoryCollector::enumerate(FileSystemContentsCallBack callback, void* userData)
{
    const Int count = m_map.getCount();

    for (Index i = 0; i < count; ++i)
    {
        const auto& pair = m_map.getAt(i);

        UnownedStringSlice path = pair.Key;
        SlangPathType pathType = SlangPathType(pair.Value);

        // Note *is* 0 terminated in the pool
        // Let's check tho
        SLANG_ASSERT(path.begin()[path.getLength()] == 0);
        callback(pathType, path.begin(), userData);
    }

    return getDirectoryExists() ? SLANG_OK : SLANG_E_NOT_FOUND;
}

} // namespace Slang