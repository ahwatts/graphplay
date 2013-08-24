// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <memory>
#include <gtest/gtest.h>

#include "../graphplay/Mesh.h"
#include "../graphplay/Scene.h"

namespace graphplay {
    TEST(SceneTest, DefaultConstructor) {
        Scene s(640, 480);
        ASSERT_EQ(0, s.getNumMeshes());

        unsigned int num = 0;
        for (auto m : s) { ++num; }
        ASSERT_EQ(0, num);
    }

    TEST(SceneTest, AddMesh) {
        Scene scene(640, 480);
        sp_Mesh mesh(new Mesh());

        scene.addMesh(mesh);

        unsigned int num = 0;
        for (auto m : scene) {
            ++num;
            ASSERT_EQ(mesh, m.lock());
        }
        ASSERT_EQ(1, num);
    }

    TEST(SceneTest, RemoveMesh) {
        Scene scene(640, 480);
        sp_Mesh mesh(new Mesh());
        scene.addMesh(mesh);
        scene.removeMesh(mesh);

        ASSERT_EQ(0, scene.getNumMeshes());
        unsigned int num = 0;
        for (auto m : scene) { ++num; }
        ASSERT_EQ(0, num);
    }
};
