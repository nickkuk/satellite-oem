#pragma once

#include "./structs.hpp"
#include "./materialSet.hpp"
#include "./samplerSet.hpp"
#include "./textureSet.hpp"
#include "./triangleHierarchy.hpp"

namespace NSM
{
    namespace rt
    {

        class Pipeline
        {
        protected:
            const size_t INTENSIVITY = 512;
            const size_t MAX_SURFACE_IMAGES = 72;
            const size_t BLOCK_COUNTER = 0;
            const size_t PREPARING_BLOCK_COUNTER = 1;
            const size_t CLEANING_COUNTER = 2;
            const size_t UNORDERED_COUNTER = 3;
            const size_t AVAILABLE_COUNTER = 4;
            const size_t HIT_COUNTER = 6;
            const size_t INTERSECTIONS_COUNTER = 7;

            // current compute device
            DeviceQueueType device;

            // compute pipelines
            ComputeContext rayGeneration, sampleCollection, surfaceShadingPpl,
                clearSamples, bvhTraverse, vertexInterp, rayShadePipeline, binCollect,
                unorderedFormer;

            // swap PTR's (now no swapping)
            BufferType indicesSwap[2];
            BufferType availableSwap[2];

            // for staging
            BufferType generalStagingBuffer;
            BufferType generalLoadingBuffer;

            // static buffers
            BufferType zerosBufferReference;
            BufferType debugOnes32BufferReference;

            // blocks
            BufferType rayNodeBuffer, rayBlockBuffer, rayIndexSpaceBuffer, blockBinBuffer, texelBuffer,
                hitBuffer, countersBuffer, unorderedTempBuffer; //

            // active blocks
            BufferType currentBlocks, preparingBlocks;

            // block where will rewrite
            BufferType clearingBlocks, availableBlocks;

            // traverse buffers
            BufferType traverseBlockData, traverseCacheData, leafTraverseBuffer,
                prepareTraverseBuffer;

            // output images
            TextureType accumulationImage, filteredImage, flagsImage, depthImage,
                normalImage, albedoImage;

            // uniforms
            std::vector<LightUniformStruct> lightUniformData;
            UniformBuffer lightUniform;

            std::vector<RayBlockUniform> rayBlockData;
            UniformBuffer rayBlockUniform;

            std::vector<RayStream> rayStreamsData;
            UniformBuffer rayStreamsUniform;

            // vk::DescriptorPool descriptorPool;

            std::vector<vk::DescriptorSet> rayTracingDescriptors, rayTraverseDescriptors, samplingDescriptors, surfaceDescriptors;
            std::vector<vk::DescriptorSetLayout> rayTracingDescriptorsLayout, rayTraverseDescriptorsLayout, samplingDescriptorsLayout, surfaceDescriptorsLayout;

            vk::PipelineLayout rayTracingPipelineLayout,
                rayTraversePipelineLayout, samplingPipelineLayout, surfacePipelineLayout;

            double starttime = 0.f;
            uint32_t canvasWidth = 1, canvasHeight = 1;
            size_t sequenceId = 0;

            std::string shadersPathPrefix = "shaders-spv";
            bool hitCountGot = false, doCleanSamples = false;

            std::shared_ptr<MaterialSet> boundMaterialSet;

        protected:
            void syncUniforms();
            void initDescriptorSets();
            void initPipelines();
            void initLights();
            void reloadQueuedRays();
            void initBuffers();
            void init(DeviceQueueType &device);


        protected:
            void clearRays();
            void generate();
            void collectSamples();
            void rayShading();
            void traverse();

        public:
            void clearSampling();
            void dispatchRayTracing();

            void setSkybox(TextureType &skybox);
            void resizeCanvas(uint32_t width, uint32_t height);
            void reallocRays(uint32_t width, uint32_t height);
            
            void setPerspective(const glm::dmat4 &persp);
            void setModelView(const glm::dmat4 &mv);

            void setSamplerSet(std::shared_ptr<SamplerSet> &samplerSet);
            void setTextureSet(std::shared_ptr<TextureSet> &textureSet);
            void setMaterialSet(std::shared_ptr<MaterialSet> &materialSet);
            void setHierarchyStorage(std::shared_ptr<HieararchyStorage> &hierarchy);

            uint32_t getCanvasWidth();
            uint32_t getCanvasHeight();

            TextureType &getRawImage();
            TextureType &getFilteredImage();
            TextureType &getAlbedoImage();
            TextureType &getNormalImage();

            // panorama mode
            void enable360mode(bool mode);
            size_t getRayCount();

        public:
            Pipeline() {}
            Pipeline(DeviceQueueType &device, std::string shadersPack)
            {
                shadersPathPrefix = shadersPack;
                init(device);
            }
        };

    } // namespace rt
} // namespace NSM