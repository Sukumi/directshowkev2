#include "DSKInitParameters.h"

DSKInitParameters::DSKInitParameters(const std::string filePathIn, DSVideoRendererType videoRendererTypeIn, DSGraphType graphTypeIn) {
	this->filePath = filePathIn;
	this->videoRendererType = videoRendererTypeIn;
	this->graphType = graphTypeIn;
}

DSKInitParameters::~DSKInitParameters() {
}

std::string DSKInitParameters::getFilePath() {
	return this->filePath;
}

DSVideoRendererType DSKInitParameters::getVideoRendererType() {
	return this->videoRendererType;
}

DSGraphType DSKInitParameters::getGraphType() {
	return this->graphType;
}