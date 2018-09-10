#pragma once

namespace z11
{
	class MIView;
	class MGSize;
	class MILayoutManager
	{
	public:
		virtual ~MILayoutManager(void);
		virtual void installed(MIView *host);
		virtual void uninstalled(MIView *host);

		virtual void layout(MIView *host)=0;
		virtual MGSize getPreferredSize(MIView* host) = 0;
	protected:
		MILayoutManager(void);
	};

}
