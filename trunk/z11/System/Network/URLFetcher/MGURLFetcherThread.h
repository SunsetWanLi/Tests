#pragma once

#include "../../Shared/Syncable.h"
#include "../../Thread/MGThread.h"
#include "../../Thread/MGSemaphore.h"
#include "MGURLFetcher.h"
#include <string>
#include <queue>
namespace z11
{
	class MGURLFetcherRequest
	{
	public:
		std::string url;
		std::string file_name;
	};
	class MGURLFetcherThread : public MGThread, public Syncable
	{
	public:
		MGURLFetcherThread();
		~MGURLFetcherThread();
		virtual void requestExit();
		void setDownloadFolder(const std::string &download_folder);
		virtual bool threadLoop();
		void addTask(MGURLFetcherRequest *request);
	private:
		MGURLFetcher fetcher;
		MGSemaphore m_fetcher_semaphore;
		std::queue<MGURLFetcherRequest *> requests;
		std::string m_download_folder;
	};
}
