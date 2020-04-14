#include "ObbTest.h"



ObbTest::ObbTest()
{
}


ObbTest::~ObbTest()
{
}

public class ObbTest : MonoBehaviour
{
	public Transform A;
	public Transform B;

	void Start()
	{
		Debug.Log(Intersects(ToObb(A), ToObb(B)));
	}

	static Obb ToObb(Transform t)
	{
		return new Obb(t.position, t.localScale, t.rotation);
	}

	class Obb
	{
		public readonly Vector3[] Vertices;
		public readonly Vector3 Right;
		public readonly Vector3 Up;
		public readonly Vector3 Forward;

		public Obb(Vector3 center, Vector3 size, Quaternion rotation)
		{
			var max = size / 2;
			var min = -max;

			Vertices = new[]
			{
				center + rotation * min,
					center + rotation * new Vector3(max.x, min.y, min.z),
					center + rotation * new Vector3(min.x, max.y, min.z),
					center + rotation * new Vector3(max.x, max.y, min.z),
					center + rotation * new Vector3(min.x, min.y, max.z),
					center + rotation * new Vector3(max.x, min.y, max.z),
					center + rotation * new Vector3(min.x, max.y, max.z),
					center + rotation * max,
			};

			Right = rotation * Vector3.right;
			Up = rotation * Vector3.up;
			Forward = rotation * Vector3.forward;
		}
	}

	static bool Intersects(Obb a, Obb b)
	{
		if (Separated(a.Vertices, b.Vertices, a.Right))
			return false;
		if (Separated(a.Vertices, b.Vertices, a.Up))
			return false;
		if (Separated(a.Vertices, b.Vertices, a.Forward))
			return false;

		if (Separated(a.Vertices, b.Vertices, b.Right))
			return false;
		if (Separated(a.Vertices, b.Vertices, b.Up))
			return false;
		if (Separated(a.Vertices, b.Vertices, b.Forward))
			return false;

		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Right, b.Right)))
			return false;
		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Right, b.Up)))
			return false;
		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Right, b.Forward)))
			return false;

		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Up, b.Right)))
			return false;
		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Up, b.Up)))
			return false;
		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Up, b.Forward)))
			return false;

		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Forward, b.Right)))
			return false;
		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Forward, b.Up)))
			return false;
		if (Separated(a.Vertices, b.Vertices, Vector3.Cross(a.Forward, b.Forward)))
			return false;

		return true;
	}

	static bool Separated(Vector3[] vertsA, Vector3[] vertsB, Vector3 axis)
	{
		// Handles the cross product = {0,0,0} case
		if (axis == Vector3.zero)
			return false;

		var aMin = float.MaxValue;
		var aMax = float.MinValue;
		var bMin = float.MaxValue;
		var bMax = float.MinValue;

		// Define two intervals, a and b. Calculate their min and max values
		for (var i = 0; i < 8; i++)
		{
			var aDist = Vector3.Dot(vertsA[i], axis);
			aMin = aDist < aMin ? aDist : aMin;
			aMax = aDist > aMax ? aDist : aMax;
			var bDist = Vector3.Dot(vertsB[i], axis);
			bMin = bDist < bMin ? bDist : bMin;
			bMax = bDist > bMax ? bDist : bMax;
		}

		// One-dimensional intersection test between a and b
		var longSpan = Mathf.Max(aMax, bMax) - Mathf.Min(aMin, bMin);
		var sumSpan = aMax - aMin + bMax - bMin;
		return longSpan >= sumSpan; // > to treat touching as intersection
	}
}