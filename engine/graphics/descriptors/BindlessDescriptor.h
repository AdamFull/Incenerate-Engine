#pragma once

#include <vulkan/vulkan.hpp>

#include <queue>
#include <unordered_set>

namespace utl
{
	template<class _Ty>
	class unique_queue
	{
	public:
		void push(const _Ty& value) 
		{
			if (_unique_values.insert(value).second)
				_q.push(value);
		}

		void pop() {
			if (!_q.empty()) 
			{
				_unique_values.erase(_q.front());
				_q.pop();
			}
		}

		const _Ty& front() const 
		{
			return _q.front();
		}

		const _Ty& back() const
		{
			return _q.back();
		}

		bool empty() const 
		{
			return _q.empty();
		}

		size_t size() const 
		{
			return _q.size();
		}
	private:
		std::queue<_Ty> _q;
		std::unordered_set<_Ty> _unique_values;
	};
}

namespace engine
{
	namespace graphics
	{
		class CBindlessDescriptor
		{
		public:
			CBindlessDescriptor(CDevice* device);
			~CBindlessDescriptor();

			void create();
			void update();

			void addTexture(size_t tex_id);

			void bind(const vk::CommandBuffer& commandBuffer, const vk::PipelineBindPoint pipelineBindPoint, const vk::PipelineLayout& pipelineLayout);

			const vk::DescriptorSet& get() const;
			const vk::DescriptorSetLayout& getDescriptorSetLayout() const;
			const vk::DescriptorPool& getDescriptorPool() const;

		private:
			void createDescriptorPool();
			void createDescriptorSetLayout();
			void createDescriptorSet();
		private:
			CDevice* pDevice{ nullptr };

			std::unordered_map<uint32_t, utl::unique_queue<size_t>> m_mTextureIds;
			std::vector<vk::DescriptorSet> m_vDescriptorSets;
			vk::DescriptorSetLayout m_pDescriptorSetLayout{ nullptr };
			vk::DescriptorPool m_pDescriptorPool{ nullptr };
		};
	}
}