#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openframeworks");

	ofBackground(0);
	ofSetLineWidth(2);
	ofEnableDepthTest();

	this->cap.open("D:\\MP4\\MOV_1030.mp4");
	this->cap_size = cv::Size(1280, 720);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	cv::Mat src;
	this->cap >> src;
	if (src.empty()) {

		this->cap.set(cv::CAP_PROP_POS_FRAMES, 0);
		return;
	}

	cv::cvtColor(src, src, cv::COLOR_BGR2RGB);
	cv::resize(src, this->frame, this->cap_size);
	cv::flip(this->frame, this->frame, 0);

	this->image.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateY(ofGetFrameNum());
	ofTranslate(-640, -360);

	int span = 40;

	vector<ofMesh> mesh_list;
	for (int x = 0; x < 1280; x += span) {

		for (int y = 0; y < 720; y += span) {

			ofMesh mesh;

			mesh.addVertex(glm::vec3(x, y, 0));
			mesh.addVertex(glm::vec3(x + span, y, 0));
			mesh.addVertex(glm::vec3(x + span, y + span, 0));
			mesh.addVertex(glm::vec3(x, y + span, 0));

			mesh.addTexCoord(glm::vec3(x, y, 0));
			mesh.addTexCoord(glm::vec3(x + span, y, 0));
			mesh.addTexCoord(glm::vec3(x + span, y + span, 0));
			mesh.addTexCoord(glm::vec3(x, y + span, 0));

			mesh.addIndex(0); mesh.addIndex(1); mesh.addIndex(2);
			mesh.addIndex(0); mesh.addIndex(2); mesh.addIndex(3);

			mesh_list.push_back(mesh);
		}
	}

	for (auto& mesh : mesh_list) {

		auto rotate = glm::vec3(ofRandom(360), ofRandom(360), ofRandom(360));
		auto speed = glm::vec3(ofRandom(5, 10), ofRandom(5, 10), ofRandom(5, 10));

		auto center = glm::vec3(mesh.getVertex(0).x + span * 0.5, mesh.getVertex(0).y + span * 0.5, 0);
		auto tmp_center = center;

		mesh.setVertex(0, mesh.getVertex(0) - center);
		mesh.setVertex(1, mesh.getVertex(1) - center);
		mesh.setVertex(2, mesh.getVertex(2) - center);
		mesh.setVertex(3, mesh.getVertex(3) - center);

		auto noise_value = ofNoise(glm::vec4(center * 0.001, ofGetFrameNum() * 0.005));
		if (noise_value < 0.48) {

			tmp_center += glm::vec3(0, 0, -250);
			rotate = glm::vec3();
		}
		else if (noise_value < 0.52) {

			tmp_center += glm::vec3(0, 0, ofMap(noise_value, 0.48, 0.52, -250, 250));
			rotate.x += ofGetFrameNum() * speed.x;
			rotate.y += ofGetFrameNum() * speed.y;
			rotate.z += ofGetFrameNum() * speed.z;
		}
		else {

			tmp_center += glm::vec3(0, 0, 250);
			rotate = glm::vec3();
		}

		ofPushMatrix();
		ofTranslate(tmp_center);
		ofRotateZ(rotate.z);
		ofRotateY(rotate.y);
		ofRotateX(rotate.x);

		ofSetColor(255);
		this->image.bind();
		mesh.draw();
		this->image.unbind();

		ofMesh line;
		line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
		if (noise_value > 0.48 && noise_value < 0.52) {

			line.addVertex(mesh.getVertex(0));
			line.addVertex(mesh.getVertex(1));
			line.addVertex(mesh.getVertex(2));
			line.addVertex(mesh.getVertex(3));

			line.addIndex(0); line.addIndex(1);
			line.addIndex(1); line.addIndex(2);
			line.addIndex(2); line.addIndex(3);
			line.addIndex(3); line.addIndex(0);
		}
		else {

			auto top_noise_value = ofNoise(glm::vec4((center + glm::vec3(0, span, 0)) * 0.001, ofGetFrameNum() * 0.005));
			if ((top_noise_value > 0.48 && top_noise_value < 0.52 || center.y >= 720 - span * 0.5) ) {

				line.addVertex(mesh.getVertex(2));
				line.addVertex(mesh.getVertex(3));

				line.addIndex(line.getNumVertices() - 1); line.addIndex(line.getNumVertices() - 2);
			}

			auto bottom_noise_value = ofNoise(glm::vec4((center - glm::vec3(0, span, 0)) * 0.001, ofGetFrameNum() * 0.005));
			if (bottom_noise_value > 0.48 && bottom_noise_value < 0.52 || center.y <= span * 0.5) {

				line.addVertex(mesh.getVertex(0));
				line.addVertex(mesh.getVertex(1));

				line.addIndex(line.getNumVertices() - 1); line.addIndex(line.getNumVertices() - 2);
			}

			auto right_noise_value = ofNoise(glm::vec4((center + glm::vec3(span, 0, 0)) * 0.001, ofGetFrameNum() * 0.005));
			if (right_noise_value > 0.48 && right_noise_value < 0.52 || center.x >= 1280 - span * 0.5) {

				line.addVertex(mesh.getVertex(1));
				line.addVertex(mesh.getVertex(2));

				line.addIndex(line.getNumVertices() - 1); line.addIndex(line.getNumVertices() - 2);
			}

			auto left_noise_value = ofNoise(glm::vec4((center -	glm::vec3(span, 0, 0)) * 0.001, ofGetFrameNum() * 0.005));
			if (left_noise_value > 0.48 && left_noise_value < 0.52 || center.x <= span * 0.5) {

				line.addVertex(mesh.getVertex(3));
				line.addVertex(mesh.getVertex(0));

				line.addIndex(line.getNumVertices() - 1); line.addIndex(line.getNumVertices() - 2);
			}
		}

		line.drawWireframe();
		

		ofPopMatrix();
	}

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}