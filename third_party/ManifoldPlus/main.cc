#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>

#include "ManifoldPlus/IO.h"
#include "ManifoldPlus/Manifold.h"
#include "ManifoldPlus/Parser.h"
#include "ManifoldPlus/types.h"

int main(int argc, char**argv) {
	Parser parser;
	parser.AddArgument("input", "../examples/input.obj");
	parser.AddArgument("output", "../examples/output.obj");
	parser.AddArgument("depth", "8");
	parser.ParseArgument(argc, argv);
	parser.Log();

	MatrixD V, out_V;
	MatrixI F, out_F;
	ReadOBJ(parser["input"].c_str(), &V, &F);

	printf("vertex number: %ld    face number: %ld\n", V.rows(), F.rows());
	int depth = 0;
	sscanf(parser["depth"].c_str(), "%d", &depth);

	Manifold manifold;
	manifold.ProcessManifold(V, F, depth, &out_V, &out_F);

	WriteOBJ(parser["output"].c_str(), out_V, out_F);

	return 0;
}