#ifndef __MESSAGESINK_H__
#define __MESSAGESINK_H__

#include <kernel/parallel/communicator.h>
#include <kernel/basic/string.h>
#include <kernel/system/sstream.h>
#include <kernel/system/string.h>

using namespace std;

/** MessageSink is used to collect MessageHelper messages and to keep them to synchronize
 * whenever it is good time to do it.
 */
class MessageSink : public Communicator{

	private:
		static ostringstream priv;
		static ostringstream all;

		/** Nonempty on a root thread. */

	public:
		static void CollectText(int when, String what);
		static int Flush();
		static void Synchronize();

};


#endif
