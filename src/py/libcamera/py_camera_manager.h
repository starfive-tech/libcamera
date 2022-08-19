/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2022, Tomi Valkeinen <tomi.valkeinen@ideasonboard.com>
 */

#pragma once

#include <mutex>

#include <libcamera/libcamera.h>

#include <pybind11/smart_holder.h>

using namespace libcamera;

class PyCameraManager
{
public:
	PyCameraManager();
	~PyCameraManager();

	pybind11::list cameras();
	std::shared_ptr<Camera> get(const std::string &name) { return cameraManager_->get(name); }

	static const std::string &version() { return CameraManager::version(); }

	int eventFd() const { return eventFd_; }

	std::vector<pybind11::object> getReadyRequests();

	void handleRequestCompleted(Request *req);

private:
	std::unique_ptr<CameraManager> cameraManager_;

	int eventFd_ = -1;
	std::mutex completedRequestsMutex_;
	std::vector<Request *> completedRequests_;

	void writeFd();
	void readFd();
	void pushRequest(Request *req);
	std::vector<Request *> getCompletedRequests();
};