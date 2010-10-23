#include <string>

enum DSVideoRendererType { VMR7, VMR9, EVR};
enum DSGraphType {OGG, INTELIGENT_CONNECT};

class DSKInitParameters {
public:
	DSKInitParameters(const std::string filePathIn, DSVideoRendererType videoRendererType, DSGraphType graphType);
	~DSKInitParameters();

	std::string getFilePath();
	DSVideoRendererType getVideoRendererType();
	DSGraphType getGraphType();
private:
	std::string filePath;
	DSVideoRendererType videoRendererType;
	DSGraphType graphType;
};
