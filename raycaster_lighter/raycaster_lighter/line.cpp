
struct Line
{
	Vert2f pos;
	Vec2f dir;
	int lenght{0};

	inline Line(Vert2f pos, Vec2f dir, int lenght) : pos(pos), dir(dir), lenght(lenght) {}

	void draw(Color color)
	{
		
		Vec2f second_point = pos + dir * lenght;
		drawLine(pos.x, pos.y, second_point.x, second_point.y, color);
	}
};