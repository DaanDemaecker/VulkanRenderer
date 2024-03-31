// TextureDescriptorObject.h
// This class wil handle the updating of texture descriptor sets

// File includes
#include "DescriptorObject.h"
#include "Structs.h"

// Standard includes
#include <initializer_list>
#include <string>


namespace D3D
{
    class TextureDescriptorObject final : public DescriptorObject
    {
    public:
        TextureDescriptorObject() = delete;

        TextureDescriptorObject(Texture& texture);

        TextureDescriptorObject(std::initializer_list<const std::string>& filePaths);

        virtual ~TextureDescriptorObject();

        virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int index) override;
    private:
        std::vector<Texture> m_Textures{};
        std::vector<VkDescriptorImageInfo> m_ImageInfos{};


        void SetupTextures(std::initializer_list<const std::string>& filePaths);

        void SetupImageInfos();
    };
}