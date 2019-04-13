#include "stl_output.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <iomanip>
#include <sstream>


using namespace std;

static const PxVec3 g_box_vertex_data[] = {
	PxVec3(-1.0f, -1.0f, -1.0f),// �O�p�`1:�J�n
	PxVec3(-1.0f, -1.0f, 1.0f),
	PxVec3(-1.0f, 1.0f, 1.0f),// �O�p�`1:�I��

	PxVec3(1.0f, 1.0f, -1.0f),// �O�p�`2:�J�n
	PxVec3(-1.0f, -1.0f, -1.0f),
	PxVec3(-1.0f, 1.0f, -1.0f),// �O�p�`2:�I��

	PxVec3(1.0f, -1.0f, 1.0f),
	PxVec3(-1.0f, -1.0f, -1.0f),
	PxVec3(1.0f, -1.0f, -1.0f),

	PxVec3(1.0f, 1.0f, -1.0f),
	PxVec3(1.0f, -1.0f, -1.0f),
	PxVec3(-1.0f, -1.0f, -1.0f),

	PxVec3(-1.0f, -1.0f, -1.0f),
	PxVec3(-1.0f, 1.0f, 1.0f),
	PxVec3(-1.0f, 1.0f, -1.0f),

	PxVec3(1.0f, -1.0f, 1.0f),
	PxVec3(-1.0f, -1.0f, 1.0f),
	PxVec3(-1.0f, -1.0f, -1.0f),

	PxVec3(-1.0f, 1.0f, 1.0f),
	PxVec3(-1.0f, -1.0f, 1.0f),
	PxVec3(1.0f, -1.0f, 1.0f),

	PxVec3(1.0f, 1.0f, 1.0f),
	PxVec3(1.0f, -1.0f, -1.0f),
	PxVec3(1.0f, 1.0f, -1.0f),

	PxVec3(1.0f, -1.0f, -1.0f),
	PxVec3(1.0f, 1.0f, 1.0f),
	PxVec3(1.0f, -1.0f, 1.0f),

	PxVec3(1.0f, 1.0f, 1.0f),
	PxVec3(1.0f, 1.0f, -1.0f),
	PxVec3(-1.0f, 1.0f, -1.0f),

	PxVec3(1.0f, 1.0f, 1.0f),
	PxVec3(-1.0f, 1.0f, -1.0f),
	PxVec3(-1.0f, 1.0f, 1.0f),

	PxVec3(1.0f, 1.0f, 1.0f),
	PxVec3(-1.0f, 1.0f, 1.0f),
	PxVec3(1.0f, -1.0f, 1.0f)
};

static const PxVec3 g_box_normal_data[] = {
	PxVec3(-1.0f, 0.0f, 0.0f),
	PxVec3(0.0f, 0.0f, -1.0f),
	PxVec3(0.0f, -1.0f, 0.0f),
	PxVec3(0.0f, 0.0f, -1.0f),
	PxVec3(-1.0f, 0.0f, 0.0f),
	PxVec3(0.0f, -1.0f, 0.0f),
	PxVec3(0.0f, 0.0f, 1.0f),
	PxVec3(1.0f, 0.0f, 0.0f),
	PxVec3(1.0f, 0.0f, 0.0f),
	PxVec3(0.0f, 1.0f, 0.0f),
	PxVec3(0.0f, 1.0f, 0.0f),
	PxVec3(0.0f, 0.0f, 1.0f)
};


// stl�t�@�C���������o��
// output_path: �o�͐�f�B���N�g��
// actor_buffer: �A�N�^�[�����i�[�����o�b�t�@
// actor_cnt: �o�b�t�@�Ɋ܂܂��A�N�^�[��
// divide_file:
//  false�ɂ���ƑS�Ă�actor����1��STL�t�@�C���ɂ��ďo��
//  true�ɂ����1��actor�ɂ�1��STL�t�@�C�����o��
void StlOutput::outputStl(string output_path, PxActor** actor_buffer, PxU32 actor_cnt, bool divide_file)
{
	cout << "STL�t�@�C���������c" << endl;
	if (divide_file) {
		cout << "�e���̂��ʂ�STL�t�@�C���Ƃ��ďo��" << endl;
	}
	else {
		cout << "�S���̂��܂Ƃ߂�STL�t�@�C���Ƃ��ďo��" << endl;
		write_stream.open(output_path + "output.stl", ios::out);
	}

	size_t triangle_mesh_cnt = 0;

	for (PxU32 i = 0; i < actor_cnt; i++)
	{
		if (divide_file) {
			stringstream fileName;
			fileName << i << ".stl";
			write_stream.open(output_path + fileName.str(), ios::out);
			write_stream << "solid" << endl;
		}

		PxRigidActor* rigid_actor = (PxRigidActor*)actor_buffer[i];	// �A�N�^�[�̎擾
		triangle_mesh_cnt += writeRigidActor(rigid_actor);

		if (divide_file)
		{
			write_stream << "endsolid" << endl;
			write_stream.close();
		}
	}

	cout << "\t�����o���A�N�^�[��:\t " << actor_cnt << endl;
	cout << "\t�O�p�`���b�V����:\t " << triangle_mesh_cnt << endl;
	cout << "�����o������" << endl;
}

size_t StlOutput::writeRigidActor(PxRigidActor* actor)
{
	// shape���擾
	PxShape** shapes;
	shapes = new PxShape*[actor->getNbShapes()];
	actor->getShapes(shapes, actor->getNbShapes());
	PxShape* shape = shapes[0];

	// transform���擾
	PxTransform transform = PxShapeExt::getGlobalPose(*shape, *actor);

	// geometry_type���擾
	PxGeometryType::Enum geometry_type = shape->getGeometryType();


	if (geometry_type == PxGeometryType::eBOX) {
		PxBoxGeometry box;
		shape->getBoxGeometry(box);
		return writeBox(transform, box.halfExtents);
	}
	else if (geometry_type == PxGeometryType::eSPHERE)
	{
		PxSphereGeometry sphere;
		shape->getSphereGeometry(sphere);
		return writeSphere(transform, sphere.radius);
	}
	else {
		cout << "���Ή��̌`��" << endl;
		return 0;
	}
}


size_t StlOutput::writeBox(const PxTransform &transform, const PxVec3 &scale)
{
	PxMat44 matS = PxMat44(PxVec4(scale, 1.0f));//�g��k���s��
	PxMat44 matR = PxMat44(transform); //���i�E��]�s��
	PxMat44 model_matrix = matR * matS;//�g��k������]�����s�ړ��̏���

	vector<Triangle> triangles;

	for (size_t i = 0; i != 12; i++)
	{
		PxVec3* triangle_vertices = new PxVec3[3];
		// g_box_vertex_data����box���\������O�p�`�̂���1�����o���A�A�t�B���ϊ������������XYZ���W�����o���B
		triangle_vertices[0] = model_matrix.transform(PxVec4(g_box_vertex_data[i * 3], 1)).getXYZ();
		triangle_vertices[1] = model_matrix.transform(PxVec4(g_box_vertex_data[i * 3 + 1], 1)).getXYZ();
		triangle_vertices[2] = model_matrix.transform(PxVec4(g_box_vertex_data[i * 3 + 2], 1)).getXYZ();

		triangles.push_back(Triangle(g_box_normal_data[i], triangle_vertices));
	}

	return writeSolid(triangles);
}


size_t StlOutput::writeSphere(const PxTransform &transform, const PxReal radius)
{
	PxMat44 matS = PxMat44(PxVec4(PxVec3(radius), 1.0f));//�g��k���s��
	PxMat44 matR = PxMat44(transform); //���i�E��]�s��
	PxMat44 model_matrix = matR * matS;//�g��k������]�����s�ړ��̏���

	vector<Triangle> triangles;

	//���a1�̋��̒��_�E�@�����擾
	vector<PxVec3> vertices;
	vector<PxVec3> normals;
	vector<short> indices;
	const int kRings = 20;
	const int kSectors = 20;
	createSphereVerticesAndNormals(vertices, normals, indices, kRings, kSectors);
	for (size_t i = 0; i != indices.size() - 4; i += 4)
	{
		// triangle0: 2-1-0
		PxVec3* triangle_vertices_0 = new PxVec3[3];
		triangle_vertices_0[0] = model_matrix.transform(vertices[indices[i + 2]]);
		triangle_vertices_0[1] = model_matrix.transform(vertices[indices[i + 1]]);
		triangle_vertices_0[2] = model_matrix.transform(vertices[indices[i + 0]]);
		PxVec3 normal0 = 
			  (normals[indices[i + 0]] + normals[indices[i + 1]] + normals[indices[i + 2]])/3.0f;
		triangles.push_back(Triangle(normal0, triangle_vertices_0));

		// triangle1: 3-2-0
		PxVec3* triangle_vertices_1 = new PxVec3[3];
		triangle_vertices_1[0] = model_matrix.transform(vertices[indices[i + 3]]);
		triangle_vertices_1[1] = model_matrix.transform(vertices[indices[i + 2]]);
		triangle_vertices_1[2] = model_matrix.transform(vertices[indices[i + 0]]);
		PxVec3 normal1 =
			(normals[indices[i + 0]] + normals[indices[i + 2]] + normals[indices[i + 2]]) / 3.0f;

		triangles.push_back(Triangle(normal1, triangle_vertices_1));
	}
	return writeSolid(triangles);
}

size_t StlOutput::writeSolid(const vector<Triangle> &triangles)
{
	//�����o��
	for (size_t i = 0; i != triangles.size(); i++)
	{
		writeFacetNormal(triangles[i].normal_, triangles[i].vertices_ptr_);
		delete triangles[i].vertices_ptr_;
	}
	return triangles.size();
}

// ���̒��_�C�@���𓾂�֐�
// http://stackoverflow.com/questions/7946770/calculating-a-sphere-in-opengl
void StlOutput::createSphereVerticesAndNormals(vector<PxVec3> &sphere_vertices, vector<PxVec3> &sphere_normals, vector<short> &sphere_indices, int rings, int sectors)
{
	float radius = 1.0; //���a1
	float const kR = 1. / (float)(rings - 1);
	float const kS = 1. / (float)(sectors - 1);
	int r, s;

	sphere_vertices.resize(rings * sectors * 3);
	sphere_normals.resize(rings * sectors * 3);

	vector<PxVec3>::iterator v = sphere_vertices.begin();
	vector<PxVec3>::iterator n = sphere_normals.begin();

	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		float const ky = sin(-M_PI_2 + M_PI * r * kR);
		float const kx = cos(2 * M_PI * s * kS) * sin(M_PI * r * kR);
		float const kz = sin(2 * M_PI * s * kS) * sin(M_PI * r * kR);

		*v++ = PxVec3(kx, ky, kz) * radius;

		*n++ = PxVec3(kx, ky, kz);
	}

	vector<short>::iterator i = sphere_indices.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		sphere_indices.push_back(r * sectors + s);
		sphere_indices.push_back(r * sectors + (s + 1));
		sphere_indices.push_back((r + 1) * sectors + (s + 1));
		sphere_indices.push_back((r + 1) * sectors + s);

		if (((r + 1) * sectors + (s + 1)) == sphere_vertices.size() / 3) {
			return;
		}
	}
}

void StlOutput::writeFacetNormal(PxVec3 normal, PxVec3 *vertices)
{
	write_stream << "facet normal " << normal.x << " " << normal.y << " " << normal.z << endl;
	writeOuterLoop(vertices);
	write_stream << "endfacet" << endl;
}

void StlOutput::writeOuterLoop(PxVec3 *vertices)
{
	write_stream << "outer loop" << endl;
	for (size_t i = 0; i != 3; i++)
		writeVertex(vertices[i]);
	write_stream << "endloop" << endl;
}

void StlOutput::writeVertex(PxVec3 vertex)
{
	write_stream << "vertex " << fixed << setprecision(3) << vertex.x << " " << vertex.y << " " << vertex.z << endl;
}
