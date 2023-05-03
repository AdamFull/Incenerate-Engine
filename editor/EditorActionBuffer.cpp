#include "EditorActionBuffer.h"

using namespace engine::editor;

void CEditorActionBuffer::addOperation(std::unique_ptr<IOperation>&& operation)
{
	undo_stack_.push(std::move(operation));
	redo_stack_ = {};
}

void CEditorActionBuffer::undo()
{
	if (!undo_stack_.empty()) 
	{
		auto operation = std::move(undo_stack_.top());
		undo_stack_.pop();
		operation->undo();
		redo_stack_.push(std::move(operation));
	}
}

void CEditorActionBuffer::redo()
{
	if (!redo_stack_.empty()) 
	{
		auto operation = std::move(redo_stack_.top());
		redo_stack_.pop();
		operation->redo();
		undo_stack_.push(std::move(operation));
	}
}

bool CEditorActionBuffer::canUndo()
{
	return !undo_stack_.empty();
}

bool CEditorActionBuffer::canRedo()
{
	return !redo_stack_.empty();
}