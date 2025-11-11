#include "PointsObject.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "common/shader.hpp"
#include <cmath>

PointsObject::PointsObject(const std::vector<glm::vec3>& initPositions, const std::vector<glm::vec3>& initColors) {
    if (initPositions.size() != initColors.size()) {
        std::cerr << "Error: positions and colors vectors must have the same size." << std::endl;
        return;
    }
    offsetlessPositions = initPositions;
    positions = initPositions;
    colors = initColors;
    baseColors = initColors;
    bezierPositions = initPositions;
    SubdivideStart();
    CalculateBezier();

    // Generate VAO and two VBOs (one for positions, one for colors)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_colors);
    glGenBuffers(1, &VBO_subdivisionPositions);
    glGenBuffers(1, &VBO_bezierPositions);

    // Load the shader programs.
    shaderProgram = LoadShaders("pointVertexShader.glsl", "pointFragmentShader.glsl");
    pickingShaderProgram = LoadShaders("pickingPointVertexShader.glsl", "pickingPointFragmentShader.glsl");

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_subdivisionPositions);
    glBufferData(GL_ARRAY_BUFFER, subdivisionPositions.size() * sizeof(glm::vec3), subdivisionPositions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_bezierPositions);
    glBufferData(GL_ARRAY_BUFFER, bezierPositions.size() * sizeof(glm::vec3), bezierPositions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindVertexArray(2);

}



PointsObject::~PointsObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &VBO_colors);
    glDeleteBuffers(1, &VBO_subdivisionPositions);
    glDeleteBuffers(1, &VBO_bezierPositions);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(pickingShaderProgram);
}

void PointsObject::CalculateBezier() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_bezierPositions);

    bezierPositions.clear();
    for (size_t i = 0; i < positions.size(); i++) {
        glm::vec3 p0 = positions[i];
        glm::vec3 p1 = positions[(i + 1) % positions.size()];
        glm::vec3 p2 = positions[(i + 2) % positions.size()];

        // calculate halfway points
        glm::vec3 h01 = 0.5f * p0 + 0.5f * p1;
        glm::vec3 h02 = 0.5f * p1 + 0.5f * p2;

        for (int j = 0; j <= n; j++) {
            float t = (float)(j) / n;
            bezierPositions.push_back(deCasteljau(h01, p1, h02, t));
        }
    }


    glBindBuffer(GL_ARRAY_BUFFER, VBO_bezierPositions);
    glBufferData(GL_ARRAY_BUFFER, bezierPositions.size() * sizeof(glm::vec3), bezierPositions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3);
}

glm::vec3 PointsObject::deCasteljau(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t) {
    glm::vec3 p01 = (1 - t) * p0 + t * p1;
    glm::vec3 p12 = (1 - t) * p1 + t * p2;
    return (1 - t) * p01 + t * p12;
}



void PointsObject::SubdivideStart() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_subdivisionPositions);
    subdivisionPositions = positions;
    glBindBuffer(GL_ARRAY_BUFFER, VBO_subdivisionPositions);
    glBufferData(GL_ARRAY_BUFFER, subdivisionPositions.size() * sizeof(glm::vec3), subdivisionPositions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    for (int i = 0; i < k; i++) {
        Subdivide();
    }
}

void PointsObject::Subdivide() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_subdivisionPositions);

    std::vector<glm::vec3> newPositions;
    for (size_t i = 0; i < subdivisionPositions.size()-1; i++) {

        glm::vec3 point1_4 = (subdivisionPositions[i] *0.75f) + (subdivisionPositions[i + 1]) * 0.25f;
        glm::vec3 point3_4 = (subdivisionPositions[i]*0.25f) + (subdivisionPositions[i + 1]) * 0.75f;
        newPositions.push_back(point1_4);
        newPositions.push_back(point3_4);
        
    }

    // If the positions form a loop, add the points at 1/4 and 3/4 between the last and first points
    if (subdivisionPositions.size() > 1) {
        glm::vec3 point1_4 = subdivisionPositions[0]*0.25f + (subdivisionPositions[subdivisionPositions.size()-1]) * 0.75f;
        glm::vec3 point3_4 = subdivisionPositions[0]* 0.75f + (subdivisionPositions[subdivisionPositions.size()-1]) * 0.25f;
        newPositions.push_back(point1_4);
        newPositions.push_back(point3_4);
    }

    // Update the positions vector with the new subdivided points
    subdivisionPositions = newPositions;

    glBindBuffer(GL_ARRAY_BUFFER, VBO_subdivisionPositions);
    glBufferData(GL_ARRAY_BUFFER, subdivisionPositions.size() * sizeof(glm::vec3), subdivisionPositions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void PointsObject::updatePoint(int index, const glm::vec3& newPosition) {
    if (index < 0 || index >= positions.size()) {
        std::cerr << "Error: point index out of range." << std::endl;
        return;
    }

    if (!editingZ) {
        positions[index] = glm::vec3(newPosition.x, newPosition.y, positions[index].z);
    }
    else {
        positions[index] = glm::vec3(positions[index].x, newPosition.y, newPosition.z);
    }

    /*offsetlessPositions = positions;
    for each(glm::vec3 p in offsetlessPositions) {
        p.y += yOffset;
    }*/

    //printf("position 0: %f, %f, %f", positions[0].x, positions[0].y, positions[0].z);
    for (int i = 0; i < baseColors.size(); i++) {
        colors[i] = baseColors[i];
    }
    colors[index] = glm::vec3( 1.0f,1.0f,1.0f );

    SubdivideStart();
    CalculateBezier();

    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_subdivisionPositions);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_bezierPositions);

    GLintptr offset = index * sizeof(glm::vec3), size = sizeof(glm::vec3);

    glNamedBufferSubData(VBO_positions, offset, size, &positions[index]);
    glNamedBufferSubData(VBO_colors, 0, colors.size() * sizeof(glm::vec3), colors.data());
    glNamedBufferSubData(VBO_subdivisionPositions, 0, subdivisionPositions.size() * sizeof(glm::vec3), subdivisionPositions.data());
    glNamedBufferSubData(VBO_bezierPositions, 0, bezierPositions.size() * sizeof(glm::vec3), bezierPositions.data());

    //TODO: P2aTask3 - Use glBufferSubData to updated the point location in the buffer.
    // Offset = index * sizeof(glm::vec3), size = sizeof(glm::vec3)
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_subdivisionPositions);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_bezierPositions);


}


// Draw the points normally.
void PointsObject::draw(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    glm::mat4 MVP = projection * view;
    GLuint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, positions.size());
    
    if (drawBezier) {
        GLuint drawingBezier = glGetUniformLocation(shaderProgram, "drawingBezier");
        glUniform1i(drawingBezier, GL_TRUE);
        glDrawArrays(GL_LINE_LOOP, 0, bezierPositions.size()); 
        glUniform1i(drawingBezier, GL_FALSE);
    }
    
    if (animatingPoint) {
        GLuint animatingPointInt = glGetUniformLocation(shaderProgram, "animatingPoint");
        glUniform1i(animatingPointInt, GL_TRUE);
        GLuint currentPointInt = glGetUniformLocation(shaderProgram, "currentPoint");

        glUniform3f(currentPointInt, bezierPositions[currentPoint].x, bezierPositions[currentPoint].y, bezierPositions[currentPoint].z);
        //glUniform3fv(animatingPointInt, 1, glm::value_ptr(bezierPositions[currentPoint]));
        glDrawArrays(GL_POINTS, 0, 3);
        glUniform1i(animatingPointInt, GL_FALSE);
        //currentPoint = (currentPoint+1)%bezierPositions.size();
    }




    GLuint uniformYOffset = glGetUniformLocation(shaderProgram, "yOffset");
    glUniform1f(uniformYOffset, yOffset);

    GLuint drawingSubdivision = glGetUniformLocation(shaderProgram, "drawingSubdivision");
    glUniform1i(drawingSubdivision, GL_TRUE);
    glDrawArrays(GL_LINE_LOOP, 0, subdivisionPositions.size());
    glUniform1i(drawingSubdivision, GL_FALSE);

    GLuint drawingLine = glGetUniformLocation(shaderProgram, "drawingLine");
    glUniform1i(drawingLine, GL_TRUE);
    glDrawArrays(GL_LINE_LOOP, 0, positions.size());
    glUniform1i(drawingLine, GL_FALSE);

    glBindVertexArray(0);
    glUseProgram(0);
}

// Draw the points for picking.
void PointsObject::drawPicking(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(pickingShaderProgram);
    glm::mat4 MVP = projection * view;
    GLuint mvpLoc = glGetUniformLocation(pickingShaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

    glBindVertexArray(VAO);
    // In the picking shader, we rely on gl_VertexID to generate a unique color per point.
    glDrawArrays(GL_POINTS, 0, positions.size());
    glBindVertexArray(0);
    glUseProgram(0);
}

void PointsObject::animatePoint() {
    animatingPoint = !animatingPoint;
    //currentPoint = 0;
    if (animatingPoint) {
        currentPoint = 0;
    }
}

