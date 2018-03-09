/* Console example — declarations of command registration functions.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once
#define CMD_STORAGE
// Register system functions
#if defined(CMD_SYSTEM)
void register_system();
#endif // DEBUG

#if defined(CMD_STORAGE)
void register_storge();
#endif // CMD_STORAGE


