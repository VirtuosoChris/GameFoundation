#pragma once
namespace Virtuoso
{
    namespace GameFoundations
    {
        struct UpdateEvent
        {
            double timestamp = -1.;
            std::function<void(void)> payload;

            bool operator<(const UpdateEvent& other) const
            {
                return timestamp < other.timestamp;
            }

            bool operator>(const UpdateEvent& other) const
            {
                return timestamp > other.timestamp;
            }

            bool operator==(const UpdateEvent& other) const
            {
                return timestamp == other.timestamp;
            }
        };

        typedef std::priority_queue<UpdateEvent, std::vector<UpdateEvent>, std::greater<UpdateEvent>> UpdateQueue;

        inline int processUpdateEvents(double t, UpdateQueue& q)
        {
            int eventCt = q.size();
            while (!q.empty() && q.top().timestamp <= t)
            {
                UpdateEvent e = std::move(q.top());

                q.pop();
                e.payload();
            }

            return eventCt - q.size(); // events processed
        }
    }
}