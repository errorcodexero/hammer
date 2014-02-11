#ifndef INJECTOR_H
#define INJECTOR_H

#include "../util/countup_timer.h"

namespace Injector{
	enum Status{IDLE,SHOOTING,RECOVERY};
	std::ostream& operator<<(std::ostream&,Status);

	enum Output{OUTPUT_UP,OUTPUT_DOWN};
	std::ostream& operator<<(std::ostream&,Output);

	enum Goal{START,WAIT};
	std::ostream& operator<<(std::ostream&,Goal);

	class Estimator{
		public:
		enum Location{GOING_UP,UP,GOING_DOWN,DOWN};

		private:
		Location location;
		Countup_timer timer;

		public:
		Estimator();
		void update(Time,Output);
		Location estimate()const;
		Status status()const;
		void out(std::ostream&)const;
	};
	std::ostream& operator<<(std::ostream&,Estimator::Location);
	std::ostream& operator<<(std::ostream&,Estimator);

	Output control(Estimator::Location,Goal);
}

#endif
