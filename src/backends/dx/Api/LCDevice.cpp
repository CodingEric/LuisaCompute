#pragma vengine_package vengine_directx
#include <Api/LCDevice.h>
#include <DXRuntime/Device.h>
#include <Resource/DefaultBuffer.h>
#include <Codegen/DxCodegen.h>
#include <Shader/ShaderCompiler.h>
#include <Codegen/ShaderHeader.h>
#include <Resource/RenderTexture.h>
#include <Resource/BindlessArray.h>
#include <Api/LCCmdBuffer.h>
using namespace toolhub::directx;
namespace toolhub::directx {
LCDevice::LCDevice(const Context &ctx)
    : LCDeviceInterface(ctx) {
}
void *LCDevice::native_handle() const noexcept {
    return nativeDevice.device.Get();
}
uint64_t LCDevice::create_buffer(size_t size_bytes) noexcept {
    return reinterpret_cast<uint64>(
        new DefaultBuffer(
            &nativeDevice,
            size_bytes,
            nativeDevice.defaultAllocator));
}
void LCDevice::destroy_buffer(uint64_t handle) noexcept {
    delete reinterpret_cast<DefaultBuffer *>(handle);
}
void *LCDevice::buffer_native_handle(uint64_t handle) const noexcept {
    return reinterpret_cast<DefaultBuffer *>(handle)->GetResource();
}
uint64_t LCDevice::create_texture(
    PixelFormat format,
    uint dimension,
    uint width,
    uint height,
    uint depth,
    uint mipmap_levels) noexcept {
    return reinterpret_cast<uint64>(
        new RenderTexture(
            &nativeDevice,
            width,
            height,
            TextureBase::ToGFXFormat(format),
            (TextureDimension)dimension,
            depth,
            mipmap_levels,
            true,
            nativeDevice.defaultAllocator));
}
void LCDevice::destroy_texture(uint64_t handle) noexcept {
    delete reinterpret_cast<RenderTexture *>(handle);
}
void *LCDevice::texture_native_handle(uint64_t handle) const noexcept {
    return reinterpret_cast<RenderTexture *>(handle)->GetResource();
}
uint64_t LCDevice::create_bindless_array(size_t size) noexcept {
    return reinterpret_cast<uint64>(
        new BindlessArray(&nativeDevice, size));
}
void LCDevice::destroy_bindless_array(uint64_t handle) noexcept {
    delete reinterpret_cast<BindlessArray *>(handle);
}
void LCDevice::emplace_buffer_in_bindless_array(uint64_t array, size_t index, uint64_t handle, size_t offset_bytes) noexcept {
    auto buffer = reinterpret_cast<DefaultBuffer *>(handle);
    reinterpret_cast<BindlessArray *>(array)
        ->Bind(BufferView(buffer, offset_bytes), index);
}
void LCDevice::emplace_tex2d_in_bindless_array(uint64_t array, size_t index, uint64_t handle, Sampler sampler) noexcept {
    auto tex = reinterpret_cast<RenderTexture *>(handle);
    reinterpret_cast<BindlessArray *>(array)
        ->Bind(std::pair<TextureBase const *, Sampler>(tex, sampler), index);
}
void LCDevice::emplace_tex3d_in_bindless_array(uint64_t array, size_t index, uint64_t handle, Sampler sampler) noexcept {
    emplace_tex2d_in_bindless_array(array, index, handle, sampler);
}
bool LCDevice::is_buffer_in_bindless_array(uint64_t array, uint64_t handle) const noexcept {
    return reinterpret_cast<BindlessArray *>(array)
        ->IsPtrInBindless(handle);
}
bool LCDevice::is_texture_in_bindless_array(uint64_t array, uint64_t handle) const noexcept {
    return reinterpret_cast<BindlessArray *>(array)
        ->IsPtrInBindless(handle);
}
void LCDevice::remove_buffer_in_bindless_array(uint64_t array, size_t index) noexcept {
    reinterpret_cast<BindlessArray *>(array)
        ->UnBind(BindlessArray::BindTag::Buffer, index);
}
void LCDevice::remove_tex2d_in_bindless_array(uint64_t array, size_t index) noexcept {
    reinterpret_cast<BindlessArray *>(array)
        ->UnBind(BindlessArray::BindTag::Tex2D, index);
}
void LCDevice::remove_tex3d_in_bindless_array(uint64_t array, size_t index) noexcept {
    reinterpret_cast<BindlessArray *>(array)
        ->UnBind(BindlessArray::BindTag::Tex3D, index);
}
uint64_t LCDevice::create_stream() noexcept {
    return reinterpret_cast<uint64>(
        new LCCmdBuffer(
            &nativeDevice,
            nativeDevice.defaultAllocator,
            D3D12_COMMAND_LIST_TYPE_COMPUTE));
}
void LCDevice::destroy_stream(uint64_t handle) noexcept {
    delete reinterpret_cast<LCCmdBuffer *>(handle);
}
void LCDevice::synchronize_stream(uint64_t stream_handle) noexcept {
    reinterpret_cast<LCCmdBuffer *>(stream_handle)->Sync();
}
void LCDevice::dispatch(uint64_t stream_handle, CommandList const &v) noexcept {
    reinterpret_cast<LCCmdBuffer *>(stream_handle)
        ->Execute({&v, 1});
}
void LCDevice::dispatch(uint64_t stream_handle, luisa::span<const CommandList> lists) noexcept {
    reinterpret_cast<LCCmdBuffer *>(stream_handle)
        ->Execute(lists);
}

void *LCDevice::stream_native_handle(uint64_t handle) const noexcept {
    return reinterpret_cast<LCCmdBuffer *>(handle)
        ->queue.Queue();
}
uint64_t LCDevice::create_shader(Function kernel, std::string_view meta_options) noexcept {
    static DXShaderCompiler dxCompiler;
    auto str = CodegenUtility::Codegen(kernel);
    if (str) {
        std::cout
            << "\n===============================\n"
            << str->result
            << "\n===============================\n";
        vstd::string compileString(GetHLSLHeader());
        compileString << str->result;
        auto compResult = dxCompiler.CompileCompute(
            compileString,
            true);
        compResult.multi_visit(
            [](auto &&buffer) {
                std::cout << "Compile Success!! DXIL size: " << buffer->GetBufferSize() << '\n';
            },
            [](auto &&err) {
                std::cout << err << '\n';
            });
    }
    return uint64_t();
}
void LCDevice::destroy_shader(uint64_t handle) noexcept {
}
uint64_t LCDevice::create_event() noexcept {
    return uint64_t();
}
void LCDevice::destroy_event(uint64_t handle) noexcept {
}
void LCDevice::signal_event(uint64_t handle, uint64_t stream_handle) noexcept {
}
void LCDevice::wait_event(uint64_t handle, uint64_t stream_handle) noexcept {
}
void LCDevice::synchronize_event(uint64_t handle) noexcept {
}
uint64_t LCDevice::create_mesh(uint64_t v_buffer, size_t v_offset, size_t v_stride, size_t v_count, uint64_t t_buffer, size_t t_offset, size_t t_count, AccelBuildHint hint) noexcept {
    return uint64_t();
}
void LCDevice::destroy_mesh(uint64_t handle) noexcept {
}
uint64_t LCDevice::create_accel(AccelBuildHint hint) noexcept {
    return uint64_t();
}
void LCDevice::emplace_back_instance_in_accel(uint64_t accel, uint64_t mesh, luisa::float4x4 transform, bool visible) noexcept {
}
void LCDevice::pop_back_instance_from_accel(uint64_t accel) noexcept {
}
void LCDevice::set_instance_in_accel(uint64_t accel, size_t index, uint64_t mesh, luisa::float4x4 transform, bool visible) noexcept {
}
void LCDevice::set_instance_transform_in_accel(uint64_t accel, size_t index, luisa::float4x4 transform) noexcept {
}
void LCDevice::set_instance_visibility_in_accel(uint64_t accel, size_t index, bool visible) noexcept {
}
bool LCDevice::is_buffer_in_accel(uint64_t accel, uint64_t buffer) const noexcept {
    return false;
}
bool LCDevice::is_mesh_in_accel(uint64_t accel, uint64_t mesh) const noexcept {
    return false;
}
uint64_t LCDevice::get_vertex_buffer_from_mesh(uint64_t mesh_handle) const noexcept {
    return uint64_t();
}
uint64_t LCDevice::get_triangle_buffer_from_mesh(uint64_t mesh_handle) const noexcept {
    return uint64_t();
}
void LCDevice::destroy_accel(uint64_t handle) noexcept {
}
VSTL_EXPORT_C LCDeviceInterface *CreateDevice(Context const &c) {
    return new LCDevice(c);
}
}// namespace toolhub::directx