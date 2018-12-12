typedef struct MaterialType {
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
} MaterialType;
			
// Material properties for brass
MaterialType brass = { {0.33f, 0.22f, 0.03f,1.0f},
					  {0.78f, 0.57f, 0.11f, 1.0f},
					  {0.99f, 0.91f, 0.81f, 1.0f},
					   27.8f };

MaterialType redRubber = { { 0.05f,0.0f,0.0f,1.0f },
							{ 0.5f,0.4f,0.4f,1.0f},
							{ 0.7f,0.04f,0.04f,1.0f},
							10.0f };

// Material properties for silver
MaterialType silver = { {0.19225f,  0.19225f,  0.19225f,   1.0f},
						 {0.50754f,  0.50754f,  0.50754f,  1.0f},
						 {0.508273f, 0.508273f, 0.508273f, 1.0f},
						  51.2f };

MaterialType darkWood = { {0.25f, 0.148f, 0.06475f, 1.0f},
						{0.4f, 0.2368f, 0.1036f, 1.0f},
						{0.774597f, 0.458561f, 0.200621f, 1.0f},
						76.8f };

MaterialType white_acrylic = { { 1.0f, 1.0f, 1.0f, 0.15f },
							{ 1.0f, 1.0f, 1.0f, 0.15f },
							{ 1.0f, 1.0f, 1.0f, 0.15f },
							32.0f };

MaterialType perl = { { 0.25f, 0.20725f, 0.20725f, 0.922f },
					{1.0f, 0.829f, 0.829f, 0.922f },
					{0.296648f, 0.296648f, 0.296648f, 0.922f },
					11.264f };

MaterialType copper = { { 0.19125f, 0.0735f, 0.0225f, 1.0f },
						{0.7038f, 0.27048f, 0.0828f, 1.0f },
						{0.256777f, 0.137622f, 0.086014f, 1.0f },
						12.8f};

MaterialType gold = { { 0.24725f, 0.1995f, 0.0745f, 1.0f },
					{0.75164f, 0.60648f, 0.22648f, 1.0f },
					{0.628281f, 0.555802f, 0.366065f, 1.0f },
					51.2f};

void set_material(GLenum face, MaterialType *material)
{
	glMaterialfv(face,GL_AMBIENT,material->ambient);
	glMaterialfv(face,GL_DIFFUSE,material->diffuse);
	glMaterialfv(face,GL_SPECULAR,material->specular);
	glMaterialf(face,GL_SHININESS,material->shininess);
}
