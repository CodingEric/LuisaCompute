#include <runtime/rtx/procedural_primitive.h>

namespace luisa::compute {

ProceduralPrimitive::ProceduralPrimitive(DeviceInterface *device,  BufferView<AABB> aabb, const AccelOption &option) noexcept
    : 
    // TODO
    // _aabb_buffer(buffer.handle()),
    //   _aabb_count(aabb_count),
    //   _aabb_offset(aabb_offset),
      Resource(device, Resource::Tag::PROCEDURAL_PRIMITIVE,
               device->create_procedural_primitive(option)) {
}

luisa::unique_ptr<Command> ProceduralPrimitive::build(AccelBuildRequest request) noexcept {
    return ProceduralPrimitiveBuildCommand::create(handle(), request, _aabb_buffer, _aabb_offset, _aabb_count);
}

}// namespace luisa::compute