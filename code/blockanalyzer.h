//----------------------------------------------------------------------------

///@file
///AI analyzer for finding the best place to put the block header file.
///
///@par License:
///@verbatim
///CuTe - Cubic Tetris, OpenGL 3D Tetris game clone.
///Copyright (C) 2005-06 Tomasz Nurkiewicz
///For full license text see license.txt.
///
///This program is free software; you can redistribute it and/or modify it under the terms of
///the GNU General Public License as published by the Free Software Foundation;
///either version 2 of the License, or (at your option) any later version.
///
///This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
///without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
///See the GNU General Public License for more details.
///
///You should have received a copy of the GNU General Public License along with this program;
///if not, write to the Free Software Foundation, Inc.,
///59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///@endverbatim
///
///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
///@date Jul 2005-Mar 2006

//----------------------------------------------------------------------------

#ifndef BLOCKANALYZER_H
#define BLOCKANALYZER_H

//----------------------------------------------------------------------------

#include <boost/multi_array.hpp>
#include "glengine.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///AI analyzer for finding the best place to put the current block.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class performs complex tests to find the place where the current block best fits
	///on the associated game cuboid. This class is used in demo mode to enable computer to
	///play CuTe itself or as a "cheat" during normal game to help player put the block.
	class BlockAnalyzer
		{
		private:
			///Heights factor weight.
			///Heights factor describes how high the block is counting from the bottom
			///of the cuboid. The lower it is the better - that's why the factor is negative.
			///@sa countFactor()
			static const int HEIGHTS_WEIGHT = -8;
			///Distances factor weight.
			///The distances factor describes the sum of distances between the cubes in a current block
			///and the topmost cubes on a cuboid lying beneath them. Ideally this factor is zero
			///which mean all the cubes fits ideally (there are no "wholes" which are hidden by block
			///cubes). Because those wholes are ignored by the analyzer engine, it should also
			///try to protect from producing the block positions creating such a wholes).
			///That's because this factor is so big.
			///@sa countFactor()
			static const int DISTS_WEIGHT = -256;
			///Edges fit factor weight.
			///Edges fit factor describes how does the block fit to the cubes already existing on a cuboid.
			///The engine should always choose those positions on which the block adhere to other blocks
			///as much as possible.
			///@sa countFactor()
			static const int EDGES_WEIGHT = 1;
			///Number of all block rotations to check.
			///Every block is rotated ALL_ROTATIONS times around especially choosen axises. It can be
			///mathematically proved that a 3D object which can be rotated around X, Y and Z axises by
			///the multiplication of 90 degrees can be placed in exactly 24 <b>different</b> positions.
			///@sa rotation
			///@sa checkAllPositions
			static const int ALL_ROTATIONS = 24;
			///Maximum time (in ms) for the process() function to run.
			///It takes some time to test all the possible block positions on the cuboid. If tjhe process()
			///function could run as long as it could, it would cause the whole game (including display
			///sub system) to freeze for a moment (this could be even a few seconds depending on a
			///platform speed).
			///@par Quasi-thread soulution
			///To prevent this situation BlockAnalyzer creates a pseudo-thread solution: the process()
			///function checks periodically the internal timer and if it runs more than this time
			///it interrupts and waits until it will be called again to continue processing. Thanks to
			///this approach process() function can be called many times between frames rendering and no
			///"freezing" or slowing down occurs.
			///@sa process()
			static const int ANALYSYS_MAX_TIME = 15;
			///Maximum time (in ms) for the block transformations.
			///Sometimes it happens that the analyzer finds the best position but it can't transform
			///the block any how to this position. There is not elegant way to detect such a clash
			///so I've choosen to test the transformation time. If the transformation takes to much time
			///it means that the game is stucked and should be restarted
			///@sa process() where this time is checked
			///@sa GAMEOVER state
			///@sa transformationTimer
			static const int MAX_TRANSFORMATION_TIME = 1000;

			///Stores the neccessery information while computing and transforming block position.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class stores some specific information needed to put the block in right position
			///and save the position characteristics.
			class BlockPos
				{
				private:
					///Array of rotation codes for all specific rotations.
					///There are 24 unique block rotations. The analyzer tests them one after another and if it
					///finds the one particular rotation to fit more than others, it saves its number. But
					///the problem arises: how to rotate the block from its starting position to the one
					///got in the rotation. Of course the transforming module could perform n rotations if
					///the analyzer found the nth rotation be the best. But this means that in some cases
					///the block would rotate even more than 20 times on user eyes - this would look
					///silly and take (redundantly) too much time.<br>
					///This array solves the problem: it appers that to get to the specific rotation from
					///the starting position we need not more than 3 rotations! This 24 array elements saves
					///those rotations and are used as a look-up table.
					///@sa rotations()
					///@sa rotationsCount()
					///@sa rotation
					static const std::string rotCodes[ALL_ROTATIONS];
					///Minimum block position factor.
					///At the beginning it is set to some minimum value so any block position found as first
					///(even the worst) would be saved.
					///@sa reset()
					static const int MIN_FACTOR = -2147483647;
					///Block position factor.
					///This is the value returned from BlockAnalyzer::countFactor(). It is used to compare two
					///block position found and check which of these is better.
					///@sa factor()
					///@sa reset()
					int factor_;
					///Block position X coordinate.
					///Stores the X coordinate of a block in a cuboid.
					int x_;
					///Block position Y coordinate.
					///Stores the Y coordinate of a block in a cuboid.
					int y_;
					///Block rotation number.
					///Stores the rotation number (index) at which the block should be set. To get the sequence
					///of rotations to be performed to set the block in this rotation, use rotations() method.
					///@sa rotations()
					int rotation;
					///Resets the block position.
					///Resets some fields of block position object when the object is being used again in next
					///analysis process.
					void reset()	{factor_ = MIN_FACTOR;}
					///Number of rotations to performs at specified rotation number.
					///This function is used when two block positions have the same factor: algorithm
					///chooses the one which envolves less rotation operations.
					///@param rotation Rotation index to check.
					///@return Number of rotations to perform to set the block at the position coded
					///at the given rotation. This means how many times the block must be rotated to set
					///him at the given rotation position.
					///@sa rotCodes
					///@sa rotations()
					static int rotationsCount(int rotation)	{return rotCodes[rotation].length();}
				public:
					///BlockAnalyzer has full access to BlockPos fields.
					///Functions x(), y() and factor() have only informing job - the BlockAnalyzer can
					///access all the private parts of this class as he wish.
					friend class BlockAnalyzer;
					///Returns the sequence of rotations to performs.
					///@return String containing the sequence of rotations to be performed to set the block
					///at the desired rotation position.
					///@sa rotCodes for more details
					const std::string& rotations() const	{return rotCodes[rotation];}
					///Returns the block X coordinate.
					///This function might be used by the environment to output the best block position
					///coordinates. Because the BlockAnalyzer has access to private parts of BlockPos, it does
					///not have to use this function.
					///@return X coordinate of the stored block position.
					int x() const	{return x_;}
					///Returns the block Y coordinate.
					///This function might be used by the environment to output the best block position
					///coordinates. Because the BlockAnalyzer has access to private parts of BlockPos, it does
					///not have to use this function.
					///@return Y coordinate of the stored block position.
					int y() const	{return y_;}
					///Returns the block fit factor.
					///This function might be used by the environment to output the best block fit factor.
					///Because the BlockAnalyzer has access to private parts of BlockPos, it does
					///not have to use this function.
					///@return Fit factor of the block stored in the object.
					int factor() const	{return factor_;}
				};		//class BlockPos

			///Best block position found till the time.
			///This structure stores the position ((x, y), rotation code, etc.) of the best fitting
			///block found by process() method. It stores significant data only when the processing is over
			///and the state is IDLE.
			///@sa best()
			///@sa process()
			///@sa countFactor()
			BlockPos best_;
			///Block which is examined.
			///This is a copy of a cuboid current block. This block is rotated and moved many times
			///during processing to fit the currently tested position. Also when countFactor() is called
			///it uses this block during computing.
			///@sa startProcess()
			///@sa process()
			///@sa rotateBlock(char axis)
			///@sa countFactor()
			Block block;
			///Height of the topmost cube on a cuboid.
			///Each field on this 2D array represents the height of (x, y) position on a cuboid. If there
			///are no cubes on whole (x, y) column, the value is -1. If there is one cube right at the
			///bottom, there is 0, and so on.
			///@sa countCuboidHeights() where this array is updated.
			///@sa distance()
			boost::multi_array<int, 2> cuboidHeights;
			///Counts the (x, y) cube columns heights on a cuboid.
			///It searches for the topmost cube on a cube at specified (x, y) column and saves its
			///z coordinate in cuboidHeights array. This method should be called only once (in
			///startProcess() before processing all possible rotations, because it does not change
			///until the block is saved on a cuboid).
			///@sa distance()
			void countCuboidHeights();
			///Counts the fit factor for all possible (x, y) block positions.
			///This function is called for all possible rotations (24). It loops through all the
			///(x, y) block positions and checks whether the block could be put on that place.
			///If it can, calls countFactor() to compute fit factor for the particular position.
			///It also saves the block position if it find the factor to be the highest found.<br>
			///This function is called 24 times for all possible rotations to check all possible
			///combinations of block positions and rotations.
			///@sa process()
			///@sa block
			void checkAllPositions();
			///Count the "fit factor" for the block.
			///This is the essential analyzer engine function. It computes the three sub factors (heights,
			///distances and edges) for the blocks and sums them depending on their weights.
			///@return Integral value fully describing the block position. The bigger this number
			///the better the block position. So when the engine tries to find the best block position
			///it simply calls this function for all possible positions and rotations and saves the
			///one with biggest factor.
			///@sa checkAllPositions()
			///@sa HEIGHTS_WEIGHT, DISTS_WEIGHT, EDGES_WEIGHT - sub factor weights
			int countFactor();
			///Currently examined rotation.
			///There are 24 (ALL_ROTATIONS) possible unique block rotations and the analyzer engine
			///should check all of them. This variable stores the number of currently tested rotation.
			///Because process() might be interrupted during testing, this variable can also be used to
			///know where the process stopped - so it don't test the same rotations again.
			///@sa transformRot
			///@sa process()
			int rotation;
			///Analyzer state (status).
			///Analyzer can be in many different states describing its current job. This variable stores
			///information in which state it is now - this is used oth inside the engine to determinate
			///the next step in process() and outside in classes using the analyzer through the state()
			///function.
			///@sa state()
			///@sa process()
			///@sa IDLE, PROCESSING, TRANSFORMING, GAMEOVER
			int state_;
			///Transforms (moves and/or rotates) the current block.
			///This method is called as many times as needed (till the current block will fit the best
			///position) when the best block position will be found. Thanks to it the analyzer
			///not only finds the best block position, but also moves and rotates the block to this
			///position making th elife simplier.
			///@note The function only moves the block horizontally and vertically - pushing the block
			///forward must be done explicitly if needed.
			///@sa process()
			///@sa transformFort
			///@sa TRANSFORMING
			void transformBlock();
			///Current transformation rotation.
			///When the current block is rotated multiple times during transformations in transformBlock()
			///this variable keeps track on what rotations has already been done and which are still
			///awaiting.
			///@sa BlockPos::rotations()
			///@sa rotation
			///@sa transformBlock()
			unsigned int transformRot;
			///Starts processing the new current block.
			///This method is called once every time the process of new block should start. When it is
			///called once, the process() method should be called insted (but remember startProcess()
			///calls process() for the first time itself).<br>
			///The function saves a copy of a current block in a block variable and sets up some
			///important variables.
			///@sa block
			///@sa process()
			void startProcess();
			///Counts the distance between block and cuboid.
			///The distance value tells how many times we need to push the block saved in block variable
			///forward so it contact the cuboid cubes. This function is used very often during
			///computing the fit factor.
			///@return Distance as described above.
			///@sa countFactor()
			///@sa cuboidHeights
			int distance() const;
			///Rotates processed block around sopecified axis.
			///@param axis Code of axis around which we want to rotate; those can letters 'x', 'y' or 'z'
			///both lower- and uppercase: lowercase means rotating counterclockwise and uppercase is
			///counterclockwise.
			///@throws CuTeEx when the axis code is wrong.
			///@sa rotateCurrentBlock
			///@sa process()
			void rotateBlock(char axis);
			///Count the time how long are the block transformation performing.
			///This timer is used to interrupt the analyzer and set its state to GAMEOVER if
			///the transformations are taking place for too long.
			///@sa MAX_TRANSFORMATION_TIME for more details about this mechanism.
			MyOGL::Timer transformationTimer;
		protected:
			///Rotates current block around specified axis.
			///This method is used while transforming the block to perform rotations of a current block.
			///@param axis Code of axis around which we want to rotate; see rotateBlock() for further
			///details.
			///@throws CuTeEx when the axis code is wrong.
			///@sa rotateBlock()
			virtual bool rotateCurrentBlock(char axis);
			///Parent game engine object.
			///In order to properly work, analyzer must have access to game engine data, especially
			///to the cubes saved on cuboid and cuboid sizes. Besides it gets the information about
			///current block straight from the engine and also the transformations are taking place
			///directly on a current block.
			///@sa process()
			///@sa block
			EngineExt& parent;
			///Returns currently available best block position.
			///@return Class containing some data about best block position found. It's just informing
			///function because all this information is needed internally in a analyzer during processing
			///and transforming and there is no reasons for the user to read this data.<br>
			///But it is available only for information purposes, e.g. user wants to output the analyzer
			///process.
			///@sa best_
			const BlockPos& best() const	{return best_;}
			///Changes the analyzer state.
			///@param newState New analyzer state. If the state is invalid (not one of the IDLE ... GAMEOVER)
			///throws an exception.
			///@throws CuTeEx
			virtual void state(int newState);
		public:
			///Analyzer idle state.
			///This state indicates that the analyzer has finished all the processing and transforming
			///and is waiting for the next task.<br>
			///If you call process() in this state, analyzer will automatically go into PROCESSING
			///state and start processing the current block.
			///@sa state()
			static const int IDLE = 0;
			///Analyzer processing state.
			///This state indicates that the analyzer is processing all the possible block rotations and
			///positions at the moment. It also means that the information returned by best() function
			///might not be accurate because the analyzer still can find better position.<br>
			///If the analyzer finishes processing, it will automatically go into TRANSFORMING state.
			///@sa state()
			static const int PROCESSING = 1;
			///Analyzer transforming state.
			///This state indicates that the analyzer is moving and/or rotating the current block
			///so it matches the best found position.<br>
			///When the analyzer finishes transforming by putting the block into right position, it will
			///go into IDLE state. But there might that transformations tak too much time which means
			///that the current block got stuck somewhere. This will cause going the analyzer into
			///GAMEOVER state.
			///@sa transformationTimer
			///@sa state()
			static const int TRANSFORMING = 2;
			///Analyzer game over state.
			///This state indicates that the current block got stucked while trying to move it
			///into right position. There is no other way to solve this problem than just restarting the
			///demo.
			///@sa state()
			static const int GAMEOVER = 3;

			///Makes some initialization work.
			///@param iParent Parent game engine object.
			///@param startImmediately If true analyzer will automatically start processing the current
			///block in the constructor. If false, it will only initialize and wait for explicitly call
			///to process()
			///@sa parent
			BlockAnalyzer(EngineExt& iParent, bool startImmediately = true);
			virtual ~BlockAnalyzer()	{}
			///Main class method used for continue processing.
			///This function does all the job connected to processing the current block. If you call it
			///it will start or contunue (depending on the current state) calculations or transformations.
			///<br>It also keeps eye on time it's being working and if this time exceeds ANALYSYS_MAX_TIME
			///it interrupts and waits untill it will be called again. This solution makes a quasi-thread
			///behaviour which prevents the game to "freeze" for a moment when the analyzer works for too
			///long.
			///@sa ANALYSYS_MAX_TIME
			///@sa state()
			virtual void process();
			///Returns the current analyzer state.
			///@return State constant, can be one of IDLE, PROCESSING, TRANSFORMING, GAMEOVER
			///@sa process()
			virtual int state() const	{return state_;}
		};		//class BlockAnalyzer

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define BLOCKANALYZER_H

//----------------------------------------------------------------------------
