/* Filename: mesh.h */

#ifndef MESH_HEADER
#define MESH_HEADER

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.h>

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

/* Vertex Structure */
typedef struct Vertex {
    glm::vec3 Position;  // position
    glm::vec3 Normal;    // normal
    glm::vec2 TexCoords; // texCoords
    glm::vec3 Tangent;   // tangent
    glm::vec3 Bitangent; // bitangent

    int m_BoneIDs[MAX_BONE_INFLUENCE];   // Bone indexes which will influence this vertex
    float m_Weights[MAX_BONE_INFLUENCE]; // Weights from each bone

} Vertex;

/* Texture Structure */
typedef struct Texture {
    unsigned int id;
    std::string type, path;
} Texture;

/* Mesh Class */
class Mesh {
public:
    /* Mesh Data */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures); // constructor
    void Draw(Shader& shader);                                                                            // render the mesh

private:
    /* Render Data */
    unsigned int VBO, EBO;

    void setupMesh(void); // initializes all the buffer objects/arrays
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh(); // now that we have all the required data, set the vertex buffers and its attribute pointers.
}

void Mesh::Draw(Shader& shader)
{
    // Bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
        
        // Retrieve texture number (the N in diffuse_textureN)
        std::string number, name = textures[i].type;
        
        if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
        else if (name == "texture_specular") number = std::to_string(specularNr++); // Transfer unsigned int to std::string
        else if (name == "texture_normal") number = std::to_string(normalNr++);     // Transfer unsigned int to std::string
        else if (name == "texture_height") number = std::to_string(heightNr++);     // Transfer unsigned int to std::string

        glUniform1i(glGetUniformLocation(shader.shaderProgramID, (name + number).c_str()), i); // Now set the sampler to the correct texture unit
        glBindTexture(GL_TEXTURE_2D, textures[i].id);                                          // And finally bind the texture
    }

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh(void)
{
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    /* Set the vertex attribute pointers */
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);                           // Vertex Positions
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));    // Vertex Normals
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)); // Vertex Texture Coordinates
    glEnableVertexAttribArray(3); glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));   // Vertex Tangent
    glEnableVertexAttribArray(4); glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent)); // Vertex Tangent
    glEnableVertexAttribArray(5); glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));            // Vertex Tangent
    glEnableVertexAttribArray(6); glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights)); // Weights
    
    glBindVertexArray(0);

    unsigned int lightSourceVAO;
    glGenVertexArrays(1, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
}

#endif /* MESH_HEADER */
