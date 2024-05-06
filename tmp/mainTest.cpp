#include "../Server/EchoServer.hpp"
#include "../Parse/Config.hpp"

using namespace std;

int main(int ac, char **av)
{
	Config Conf(ac, av);
	// vector<LocationConfig> abc = Conf[0].getLocation();
	// vector<LocationConfig>::iterator it = abc.begin();

	// for (; it != Conf[0].getLocation().end(); ++it)
	// {
	// 	cout << "Path: " << it->getPath() << endl;
	// 	cout << "Alias: " << it->getAlias() << endl;
	// 	cout << "Root: " << it->getRoot() << endl;
	// 	cout << "Index: " << it->getIndex() << endl;
	// 	cout << "Autoindex: " << it->getAutoindex() << endl;
	// 	cout << "Allow Methods: ";
	// 	vector<string> allow_methods = it->getAllowMethods();
	// 	for (size_t i = 0; i < allow_methods.size(); i++)
	// 		cout << allow_methods[i] << " ";
	// 	cout << endl;
	// 	cout << "Return Code: " << it->getReturnCode() << endl;
	// 	cout << "Return Url: " << it->getReturnUrl() << endl;
	// 	cout << "Fastcgi Pass: " << it->getFastcgiPass() << endl;
	// 	cout << "Upload Pass: " << it->getUploadPass() << endl;
	// 	cout << "Upload Store: " << it->getUploadStore() << endl;
	// 	cout << "Upload State Store: " << it->getUploadStateStore() << endl;
	// 	cout << "Upload Resume: " << it->getUploadResume() << endl;
	// 	cout << "Upload Max File Size: " << it->getUploadMaxFileSize() << endl;
	// 	cout << "Upload Limit Rate: " << it->getUploadLimitRate() << endl;
	// 	vector<int> upload_cleanup = it->getUploadCleanup();
	// 	cout << "Upload Cleanup: ";
	// 	for (size_t i = 0; i < upload_cleanup.size(); i++)
	// 		cout << upload_cleanup[i] << " ";
	// 	cout << endl;
	// 	vector<string> try_files = it->getTryFiles();
	// 	cout << "Try Files: ";
	// 	for (size_t i = 0; i < try_files.size(); i++)
	// 		cout << try_files[i] << " ";
	// 	cout << endl;
	// 	cout << endl;
	// 	cout << endl;
	// 	cout << endl;
	// 	cout << endl;
	// }


	// cout << "Jackson pollack dick " << Conf[1].getPath() << std::endl;
	Server Server;
	Server.makeServerSocket(Conf);
	Server.queueInit(Conf);
	Server.run(Conf);
}
