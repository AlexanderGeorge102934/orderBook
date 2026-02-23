#include <gtest/gtest.h>
#include "Pipeline.h"
#include "Stage.h"
#include <atomic>
#include <thread>
#include <chrono>

// Create a test harness class that extends Pipeline to access protected submit method
class TestPipelineHarness : public Pipeline {
public:
    template <typename T>
    void submitTask(const Stage& stage, T&& task) {
        submit(stage, std::forward<T>(task));
    }
};

class PipelineTest : public ::testing::Test {
protected:
    TestPipelineHarness pipeline;
};

// Test that pipeline initializes without crashing
TEST_F(PipelineTest, PipelineInitializesSuccessfully) {
    // Constructor should complete without throwing
    EXPECT_NO_THROW({
        TestPipelineHarness tempPipeline;
    });
}

// Test that a task submitted to Sequencer stage executes
TEST_F(PipelineTest, TaskSubmittedToSequencerExecutes) {
    std::atomic<bool> taskExecuted(false);
    
    pipeline.submitTask(Stage::Sequencer, [&taskExecuted]() {
        taskExecuted = true;
    });
    
    // Give worker thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_TRUE(taskExecuted);
}

// Test that a task submitted to Matching stage executes
TEST_F(PipelineTest, TaskSubmittedToMatchingExecutes) {
    std::atomic<bool> taskExecuted(false);
    
    pipeline.submitTask(Stage::Matching, [&taskExecuted]() {
        taskExecuted = true;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_TRUE(taskExecuted);
}

// Test that a task submitted to Logger stage executes
TEST_F(PipelineTest, TaskSubmittedToLoggerExecutes) {
    std::atomic<bool> taskExecuted(false);
    
    pipeline.submitTask(Stage::Logger, [&taskExecuted]() {
        taskExecuted = true;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_TRUE(taskExecuted);
}

// Test that multiple tasks on same stage execute in order
TEST_F(PipelineTest, MultipleTasksExecuteInFIFOOrder) {
    std::vector<int> executionOrder;
    
    pipeline.submitTask(Stage::Sequencer, [&executionOrder]() {
        executionOrder.push_back(1);
    });
    
    pipeline.submitTask(Stage::Sequencer, [&executionOrder]() {
        executionOrder.push_back(2);
    });
    
    pipeline.submitTask(Stage::Sequencer, [&executionOrder]() {
        executionOrder.push_back(3);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    EXPECT_EQ(executionOrder.size(), 3);
    if (executionOrder.size() == 3) {
        EXPECT_EQ(executionOrder[0], 1);
        EXPECT_EQ(executionOrder[1], 2);
        EXPECT_EQ(executionOrder[2], 3);
    }
}

// Test that tasks with captured values execute correctly
TEST_F(PipelineTest, TaskWithCapturedValueExecutesCorrectly) {
    int value = 42;
    int result = 0;
    
    pipeline.submitTask(Stage::Sequencer, [&result, value]() {
        result = value * 2;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_EQ(result, 84);
}

// Test that each stage processes independently
TEST_F(PipelineTest, EachStageProcessesIndependently) {
    std::atomic<int> sequencerCount(0);
    std::atomic<int> matchingCount(0);
    std::atomic<int> loggerCount(0);
    
    pipeline.submitTask(Stage::Sequencer, [&sequencerCount]() {
        sequencerCount++;
    });
    
    pipeline.submitTask(Stage::Matching, [&matchingCount]() {
        matchingCount++;
    });
    
    pipeline.submitTask(Stage::Logger, [&loggerCount]() {
        loggerCount++;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    EXPECT_EQ(sequencerCount, 1);
    EXPECT_EQ(matchingCount, 1);
    EXPECT_EQ(loggerCount, 1);
}

// Test high volume task submission
TEST_F(PipelineTest, HighVolumeTaskSubmission) {
    std::atomic<int> counter(0);
    const int NUM_TASKS = 50;
    
    for (int i = 0; i < NUM_TASKS; ++i) {
        pipeline.submitTask(Stage::Sequencer, [&counter]() {
            counter++;
        });
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    EXPECT_EQ(counter, NUM_TASKS);
}

// Test that pipeline destructor waits for threads
TEST_F(PipelineTest, DestructorWaitsForThreadCompletion) {
    std::atomic<bool> taskExecuted(false);
    
    {
        TestPipelineHarness tempPipeline;
        tempPipeline.submitTask(Stage::Sequencer, [&taskExecuted]() {
            taskExecuted = true;
        });
        // Destructor called here
    }
    
    // Task should have executed before destructor completed
    EXPECT_TRUE(taskExecuted);
}

// Test multiple tasks across different stages execute concurrently
TEST_F(PipelineTest, TasksAcrossMultipleStagesExecuteConcurrently) {
    std::vector<Stage> executedStages;
    
    pipeline.submitTask(Stage::Sequencer, [&executedStages]() {
        executedStages.push_back(Stage::Sequencer);
    });
    
    pipeline.submitTask(Stage::Matching, [&executedStages]() {
        executedStages.push_back(Stage::Matching);
    });
    
    pipeline.submitTask(Stage::Logger, [&executedStages]() {
        executedStages.push_back(Stage::Logger);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    // All stages should be represented
    EXPECT_EQ(executedStages.size(), 3);
}

// Test that task completion is consistent
TEST_F(PipelineTest, TaskCompletionIsConsistent) {
    std::atomic<int> value(0);
    
    pipeline.submitTask(Stage::Sequencer, [&value]() {
        value = 10;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(value, 10);
    
    // Run same test again
    pipeline.submitTask(Stage::Sequencer, [&value]() {
        value = 20;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(value, 20);
}