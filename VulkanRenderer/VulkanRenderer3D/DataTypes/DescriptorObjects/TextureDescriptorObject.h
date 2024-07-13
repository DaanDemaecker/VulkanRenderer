// TextureDescriptorObject.h
// This class wil handle the updating of texture descriptor sets

#ifndef TextureDescriptorObjectIncluded
#define TextureDescriptorObjectIncluded


// File includes
#include "DescriptorObject.h"
#include "DataTypes/Structs.h"

// Standard includes
#include <initializer_list>
#include <string>

namespace D3D
{
    class TextureDescriptorObject final : public DescriptorObject
    {
    public:
        // Delete default constructor
        TextureDescriptorObject() = delete;

        // Constructor
        // Parameters:
        //     texture: a pre loaded texture to be used in this object
        TextureDescriptorObject(Texture& texture);

        // Constructor
        // Parameters:
        //     filePaths: a list of filepaths to the textures to be used in this object
        TextureDescriptorObject(std::initializer_list<const std::string>& filePaths);

        // Destructor
        virtual ~TextureDescriptorObject();

        // Add the descriptor write objects to the list of descriptorWrites
        // Parameters:
        //     descriptorSet: the current descriptorset connected to this descriptor object
        //     descriptorWrites: the list of descriptorWrites this function will add to
        //     binding: the current binding in the shader files
        //     index: the current frame index of the renderer
        virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int index) override;

        Texture& GetTexture(int index = 0);

    private:
        // List of the textures
        std::vector<Texture> m_Textures{};
        // List of image info per texture
        std::vector<VkDescriptorImageInfo> m_ImageInfos{};

        // Set up a list of textures
        // Parameters:
        //     filePaths: a list of file paths pointing to the image files
        void SetupTextures(std::initializer_list<const std::string>& filePaths);

        // Set up an image info object for each texture
        void SetupImageInfos();
    };
}

#endif // !TextureDescriptorObjectIncluded