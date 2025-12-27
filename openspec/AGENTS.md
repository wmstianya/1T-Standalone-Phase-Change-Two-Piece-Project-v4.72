# OpenSpec Workflow for Embedded Projects

## Overview
OpenSpec is a structured workflow for planning and executing code changes in embedded systems projects.

## Workflow Steps

### 1. Analysis Phase
Before any code changes:
- Analyze the target module's current structure
- Document function dependencies and data flow
- Identify coupling points with other modules
- Present analysis for user confirmation

### 2. Proposal Phase
Create a change proposal document:
- Define the scope of changes
- List affected files and functions
- Describe the new architecture
- Identify risks and mitigation strategies

### 3. Execution Phase
Only after user approval:
- Implement changes incrementally
- Maintain backward compatibility during transition
- Test each module independently
- Update documentation

### 4. Verification Phase
- Run existing tests
- Manual verification on hardware
- Compare behavior before/after refactoring

## File Naming Convention
- Proposals: `openspec/proposals/YYYY-MM-DD_<feature-name>.md`
- Analysis: `openspec/analysis/<module-name>.md`

## Embedded-Specific Considerations
- Maintain interrupt-safe code
- Preserve volatile qualifiers
- Keep critical timing paths unchanged
- Test on actual hardware before finalizing

