// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_ASCENE_MRC_H_
#define FLATBUFFERS_GENERATED_ASCENE_MRC_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 25 &&
              FLATBUFFERS_VERSION_MINOR == 1 &&
              FLATBUFFERS_VERSION_REVISION == 24,
             "Non-compatible flatbuffers version included");

namespace Mrc {

struct Vertex;
struct VertexBuilder;

struct StaticMesh;
struct StaticMeshBuilder;

struct StaticModel;
struct StaticModelBuilder;

struct Scene;
struct SceneBuilder;

struct Vertex FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef VertexBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_POSITION = 4,
    VT_NORMAL = 6,
    VT_UV = 8
  };
  const ::flatbuffers::Vector<float> *position() const {
    return GetPointer<const ::flatbuffers::Vector<float> *>(VT_POSITION);
  }
  const ::flatbuffers::Vector<float> *normal() const {
    return GetPointer<const ::flatbuffers::Vector<float> *>(VT_NORMAL);
  }
  const ::flatbuffers::Vector<float> *uv() const {
    return GetPointer<const ::flatbuffers::Vector<float> *>(VT_UV);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_POSITION) &&
           verifier.VerifyVector(position()) &&
           VerifyOffset(verifier, VT_NORMAL) &&
           verifier.VerifyVector(normal()) &&
           VerifyOffset(verifier, VT_UV) &&
           verifier.VerifyVector(uv()) &&
           verifier.EndTable();
  }
};

struct VertexBuilder {
  typedef Vertex Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_position(::flatbuffers::Offset<::flatbuffers::Vector<float>> position) {
    fbb_.AddOffset(Vertex::VT_POSITION, position);
  }
  void add_normal(::flatbuffers::Offset<::flatbuffers::Vector<float>> normal) {
    fbb_.AddOffset(Vertex::VT_NORMAL, normal);
  }
  void add_uv(::flatbuffers::Offset<::flatbuffers::Vector<float>> uv) {
    fbb_.AddOffset(Vertex::VT_UV, uv);
  }
  explicit VertexBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Vertex> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Vertex>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Vertex> CreateVertex(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<float>> position = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<float>> normal = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<float>> uv = 0) {
  VertexBuilder builder_(_fbb);
  builder_.add_uv(uv);
  builder_.add_normal(normal);
  builder_.add_position(position);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Vertex> CreateVertexDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<float> *position = nullptr,
    const std::vector<float> *normal = nullptr,
    const std::vector<float> *uv = nullptr) {
  auto position__ = position ? _fbb.CreateVector<float>(*position) : 0;
  auto normal__ = normal ? _fbb.CreateVector<float>(*normal) : 0;
  auto uv__ = uv ? _fbb.CreateVector<float>(*uv) : 0;
  return Mrc::CreateVertex(
      _fbb,
      position__,
      normal__,
      uv__);
}

struct StaticMesh FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef StaticMeshBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_VERTICES = 4,
    VT_INDICES = 6
  };
  const ::flatbuffers::Vector<::flatbuffers::Offset<Mrc::Vertex>> *vertices() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<Mrc::Vertex>> *>(VT_VERTICES);
  }
  const ::flatbuffers::Vector<uint32_t> *indices() const {
    return GetPointer<const ::flatbuffers::Vector<uint32_t> *>(VT_INDICES);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_VERTICES) &&
           verifier.VerifyVector(vertices()) &&
           verifier.VerifyVectorOfTables(vertices()) &&
           VerifyOffset(verifier, VT_INDICES) &&
           verifier.VerifyVector(indices()) &&
           verifier.EndTable();
  }
};

struct StaticMeshBuilder {
  typedef StaticMesh Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_vertices(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<Mrc::Vertex>>> vertices) {
    fbb_.AddOffset(StaticMesh::VT_VERTICES, vertices);
  }
  void add_indices(::flatbuffers::Offset<::flatbuffers::Vector<uint32_t>> indices) {
    fbb_.AddOffset(StaticMesh::VT_INDICES, indices);
  }
  explicit StaticMeshBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<StaticMesh> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<StaticMesh>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<StaticMesh> CreateStaticMesh(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<Mrc::Vertex>>> vertices = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint32_t>> indices = 0) {
  StaticMeshBuilder builder_(_fbb);
  builder_.add_indices(indices);
  builder_.add_vertices(vertices);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<StaticMesh> CreateStaticMeshDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<Mrc::Vertex>> *vertices = nullptr,
    const std::vector<uint32_t> *indices = nullptr) {
  auto vertices__ = vertices ? _fbb.CreateVector<::flatbuffers::Offset<Mrc::Vertex>>(*vertices) : 0;
  auto indices__ = indices ? _fbb.CreateVector<uint32_t>(*indices) : 0;
  return Mrc::CreateStaticMesh(
      _fbb,
      vertices__,
      indices__);
}

struct StaticModel FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef StaticModelBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_MESHES = 4
  };
  const ::flatbuffers::Vector<::flatbuffers::Offset<Mrc::StaticMesh>> *meshes() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<Mrc::StaticMesh>> *>(VT_MESHES);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_MESHES) &&
           verifier.VerifyVector(meshes()) &&
           verifier.VerifyVectorOfTables(meshes()) &&
           verifier.EndTable();
  }
};

struct StaticModelBuilder {
  typedef StaticModel Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_meshes(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<Mrc::StaticMesh>>> meshes) {
    fbb_.AddOffset(StaticModel::VT_MESHES, meshes);
  }
  explicit StaticModelBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<StaticModel> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<StaticModel>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<StaticModel> CreateStaticModel(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<Mrc::StaticMesh>>> meshes = 0) {
  StaticModelBuilder builder_(_fbb);
  builder_.add_meshes(meshes);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<StaticModel> CreateStaticModelDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<Mrc::StaticMesh>> *meshes = nullptr) {
  auto meshes__ = meshes ? _fbb.CreateVector<::flatbuffers::Offset<Mrc::StaticMesh>>(*meshes) : 0;
  return Mrc::CreateStaticModel(
      _fbb,
      meshes__);
}

struct Scene FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef SceneBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_MODELS = 4
  };
  const ::flatbuffers::Vector<::flatbuffers::Offset<Mrc::StaticModel>> *models() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<Mrc::StaticModel>> *>(VT_MODELS);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_MODELS) &&
           verifier.VerifyVector(models()) &&
           verifier.VerifyVectorOfTables(models()) &&
           verifier.EndTable();
  }
};

struct SceneBuilder {
  typedef Scene Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_models(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<Mrc::StaticModel>>> models) {
    fbb_.AddOffset(Scene::VT_MODELS, models);
  }
  explicit SceneBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Scene> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Scene>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Scene> CreateScene(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<Mrc::StaticModel>>> models = 0) {
  SceneBuilder builder_(_fbb);
  builder_.add_models(models);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Scene> CreateSceneDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<Mrc::StaticModel>> *models = nullptr) {
  auto models__ = models ? _fbb.CreateVector<::flatbuffers::Offset<Mrc::StaticModel>>(*models) : 0;
  return Mrc::CreateScene(
      _fbb,
      models__);
}

inline const Mrc::Scene *GetScene(const void *buf) {
  return ::flatbuffers::GetRoot<Mrc::Scene>(buf);
}

inline const Mrc::Scene *GetSizePrefixedScene(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<Mrc::Scene>(buf);
}

inline bool VerifySceneBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<Mrc::Scene>(nullptr);
}

inline bool VerifySizePrefixedSceneBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<Mrc::Scene>(nullptr);
}

inline void FinishSceneBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<Mrc::Scene> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedSceneBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<Mrc::Scene> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace Mrc

#endif  // FLATBUFFERS_GENERATED_ASCENE_MRC_H_
