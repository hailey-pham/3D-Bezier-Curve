#ifndef POINTSOBJECT_HPP
#define POINTSOBJECT_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class PointsObject {
public:
    // Constructor takes two vectors (positions and colors) of equal length.
    PointsObject(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& colors);
    ~PointsObject();

    // Update the position of the point at the given index.
    void updatePoint(int index, const glm::vec3& newPosition);

    // Draw the points normally.
    void draw(const glm::mat4& view, const glm::mat4& projection);

    // Draw the points for picking (using a picking shader).
    void drawPicking(const glm::mat4& view, const glm::mat4& projection);

    void animatePoint();

    int k = 0;
    int n = 8;
    float yOffset = 0;
    bool drawBezier = false;
    void SubdivideStart();
    void CalculateBezier();
    bool editingZ = false;
    bool animatingPoint = false;
    int currentPoint = 0;

    std::vector<glm::vec3> bezierPositions;
private:
    // Storage for positions and colors.
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> offsetlessPositions;
    std::vector<glm::vec3> subdivisionPositions;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> baseColors;
    // OpenGL objects.
    GLuint VAO;
    GLuint VBO_positions;
    GLuint VBO_colors;
    GLuint VBO_subdivisionPositions;
    GLuint VBO_bezierPositions;

    // Shader programs.
    GLuint shaderProgram;
    GLuint pickingShaderProgram;
    
    void Subdivide();
    glm::vec3 PointsObject::deCasteljau(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t);
};

#endif // POINTSOBJECT_HPP
